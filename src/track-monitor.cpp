#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <cxxopts.hpp>
#include <fmt/core.h>
#include <ncurses.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "core/track-cache.hpp"
#include "readers/pcap/log-reader.hpp"
//#include "parserss/nmea-0183/text-log-reader.hpp"
#include "parsers/nmea0183/packet-parser.hpp"
#include "parsers/ais/parser.hpp"

#include "ui/curses-input-handler.hpp"
#include "ui/curses-renderer.hpp"

const static std::string binary_name = "trackmon";
const static std::string binary_version = "0.0.2";

static bool run = true;

// //---------------------------------------------------------
// // Procedure: OnStartUp()
// //      Note: happens before connection is open
// bool TrackMonitor::OnStartUp()
// {
//     STRING_LIST sParams;
//     STRING_LIST::iterator p;
//     double origin_latitude, origin_longitude;

//     fprintf(logfile, "==== Loading :%s: Config ====\n", GetAppName().c_str() );
//     m_MissionReader.GetConfiguration(GetAppName(), sParams);
//     for(p = sParams.begin();p!=sParams.end();p++) {
//         string line  = *p;
//         string param = tolower(biteStringX(line, '='));
//         string value = line;

//         if("latorigin" == param){
//             origin_latitude = std::atof(value.c_str());
//             fprintf(logfile, "    LatOrigin  == %g\n", origin_latitude );
//         }else if("longorigin" == param){
//             origin_longitude = std::atof(value.c_str());
//             fprintf(logfile, "    LongOrigin == %g\n", origin_longitude );
//
//         ...
//
//         cache.set_origin(origin_latitude, origin_longitude);
//     }

//     return(true);
// }


int main(int argc, char *argv[]){
    // Create a cxxopts::Options instance.
    cxxopts::Options options("trackgest", "ingest some tracks, and debug the result");
    options.add_options()
        ("l,limit", "limit processing to this many packets.  0 (default) processes all traffic.", cxxopts::value<int>()->default_value("0"))
        ("h,help", "Print usage")
        ("v,verbose", "Verbose output")
        ("V,Version", "Print Version")
    ;
    const auto clargs = options.parse(argc, argv);

    if( clargs["Version"].as<bool>() ){
        std::cout << binary_name << "    Version: " << "0.0.1-beta" << std::endl;
        exit(0);
    }

    // Set global log level to debug
    if( 1 < clargs["verbose"].count()){
        spdlog::set_level(spdlog::level::trace);
    }else if( 0 < clargs["verbose"].count()){
        spdlog::set_level(spdlog::level::debug);
    }else{
        spdlog::set_level(spdlog::level::info);
    }
    spdlog::debug("::verbosity = {} => {}", clargs["verbose"].count(), spdlog::get_level() );

    // change log pattern
    // spdlog::set_pattern("[%H:%M:%S %z] [%n][%v]");
    //
    // // create color multi threaded logger
    // auto console = spdlog::stdout_color_mt("console");
    // auto err_logger = spdlog::stderr_color_mt("stderr");
    //

    // ===========================================================================================
    spdlog::info(">>> .A. Creating Track Database:");
    TrackCache cache;

    // ===========================================================================================
    spdlog::info(">>> .B. Creating Connectors:");
    
    // const std::string ais_file = "data/ais.nmea0183.2022-05-18.log";
    // const std::string ais_file = "data/ais.nmea0183.2022-05-19.log";
    // spdlog::info("    :> Creating File Connector to:" << ais_file);
    // TextLogReader reader( ais_file );

    const std::string ais_file = "data/ais.tcpdump.2022-05-18.pcap";
    spdlog::info("    :> Creating File Connector to: {}", ais_file);
    readers::pcap::LogReader reader( ais_file );
    reader.set_filter_udp();
    reader.set_filter_port(4003);
    if( ! (reader.good()) ){
        spdlog::error("!!! Could not create all connectors");
        return EXIT_FAILURE;
    }

    // ===========================================================================================
    spdlog::info(">>> .C. Creating Parsers:");
    spdlog::info("    :> Creating AIS Parser...");
    parsers::nmea0183::PacketParser nmea_parser;
    parsers::ais::Parser ais_parser;

    // ===========================================================================================
    spdlog::info(">>> .D. Building UI: ");
    CursesInputHandler handler(cache);
    handler.update(true);

    // ===========================================================================================

    // uint32_t interval_update_count;
    // uint32_t interval_start;

    using clock = std::chrono::system_clock;
    const std::chrono::milliseconds render_blackout(20);  // wait at least this much time between render calls

    auto last_render_timestamp = clock::now();
    auto last_change_timestamp = last_render_timestamp;
    while(run){
        // .1. get next data chunk
        const auto chunk = reader.next();
        if( 0 < chunk.length ){
            // // .2. Load next chunk into parser
            // parser.load( chunk.timestamp, chunk.buffer, chunk.length );
            // // .3. Drain reports from each parser
            //Report* report = parser.parse();
            // while( report ){
            //     // Update cache
            //     last_change_timestamp = clock::now();
            //     cache.update( *report );
            //     // Pull next report
            //     report = parser.parse();
            // }
        }

        bool pending_changes = false;
        // (a) check if any update exists since the last render...
        if( last_render_timestamp < last_change_timestamp ){
            auto render_age = clock::now() - last_render_timestamp;
            if( render_blackout < render_age ){
                pending_changes = true;
            }
        }
        // last_change_timestamp = std::chrono::system_clock::now();
        //     auto current_timestamp = std::chrono::system_clock::now();


        last_render_timestamp = handler.update( pending_changes );
        sleep(0.01);
    }

    spdlog::info( cache.to_string());

    return EXIT_SUCCESS;
}

