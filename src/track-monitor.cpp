// Standard Library Includes
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Dependency Includes
#include <cxxopts.hpp>
#include <fmt/core.h>
#include <ncurses.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Project includes
#include "core/track-cache.hpp"
#include "readers/pcap/log-reader.hpp"
#include "parsers/ais/parser.hpp"
#include "parsers/moos/message-parser.hpp"
#include "parsers/moos/packet-parser.hpp"
#include "parsers/nmea0183/packet-parser.hpp"

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

void print_build_information() {
    std::cout << "==== Build Information: ==== \n";

    // Debug vs Release
#ifdef DEBUG
    std::cout << "    ::Build Type: Debug\n";
#endif
#ifdef NDEBUG
    std::cout << "    ::Build Type: Release\n";
#endif

    // Optional Modules:
#ifdef ENABLE_AIS
    std::cout << "    ::Enable: ENABLE_AIS\n";
#endif
#ifdef ENABLE_MOOS
    std::cout << "    ::Enable: ENABLE_MOOS\n";
#endif

    std::cout << std::endl;
}

void print_version_information() {
    std::cout << binary_name << "    Version: " << "0.0.1-beta" << std::endl;
}

int main(int argc, char *argv[]){
    // Create a cxxopts::Options instance.
    cxxopts::Options options("trackmon", "monitor tracks from data streams");
    options.add_options()
        ("b,build", "Display Build Information")
        ("h,help", "Print usage")
        ("v,verbose", "Verbose output")
        ("V,version", "Print Version");
    const auto clargs = options.parse(argc, argv);

    if( clargs["help"].as<bool>() ){
        std::cout << options.help() << std::endl;
        exit(0);
    }else if( clargs["version"].as<bool>() ){
        print_version_information();
        exit(0);
    }else if( clargs["build"].as<bool>() ){
        print_build_information();
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
    
#ifdef ENABLE_AIS
    // // const std::string ais_file = "data/ais.nmea0183.2022-05-18.log";
    // // const std::string ais_file = "data/ais.nmea0183.2022-05-19.log";
    // const std::string input_pcap_file = "data/ais.tcpdump.2022-05-18.pcap";
#endif

#ifdef ENABLE_MOOS
    // const std::string input_pcap_file = "data/m2_berta.moos.pcap";
    // const std::string input_pcap_file = "data/m2_berta.moos.gt140.pcap";
    const std::string input_pcap_file = "data/m2_berta.moos.p9000.pcap";
#endif

    spdlog::info("    :> Creating File Connector to: {}", input_pcap_file);
    readers::pcap::LogReader reader( input_pcap_file );
    if( ! (reader.good()) ){
        spdlog::error("!!! Could not create all connectors");
        return EXIT_FAILURE;
    }

#ifdef ENABLE_AIS
    reader.set_filter_udp();
    reader.set_filter_port(4003);
#endif 
#ifdef ENABLE_MOOS
    reader.set_filter_tcp();
    reader.set_filter_port(9000);
#endif

    // ===========================================================================================
    spdlog::info(">>> .C. Creating Parsers:");
#ifdef ENABLE_AIS
    spdlog::info("    :> Creating AIS Parser...");
    parsers::nmea0183::PacketParser nmea_parser;
    parsers::ais::Parser ais_parser;
#endif

#ifdef ENABLE_MOOS
    spdlog::info( "    >> Creating MOOS Parser..." );
    parsers::moos::PacketParser moos_packet_parser;
    spdlog::info( "    >> Creating Node-Report Parser..." );
    parsers::moos::MessageParser moos_report_parser;
#endif

    // ===========================================================================================
    spdlog::info(">>> .D. Building UI: ");
    CursesInputHandler handler(cache);
    handler.update(true);

    // ===========================================================================================

    uint32_t interval_update_count;
    uint32_t interval_start;

    using clock = std::chrono::system_clock;
    const std::chrono::milliseconds render_blackout(20);  // wait at least this much time between render calls

    auto last_render_timestamp = clock::now();
    auto last_change_timestamp = last_render_timestamp;
    while(run){
        // .1. get next data chunk
        const auto chunk = reader.next();
        if( 0 < chunk.length ){

            // .2. Load next chunk into parser
            moos_packet_parser.load( &chunk );

            // .3. Pull reports from packet until empty
            while( ! moos_packet_parser.empty() ){
                const std::string line = moos_packet_parser.next();
                if( line.empty() ){
                    continue;
                }

                // .3. Pull reports out of parser until drained
                Report* report = moos_report_parser.parse( line );
                if( report ){
                    cache.update( *report );
                    last_change_timestamp = clock::now();
                    ++interval_update_count;
                }
            }
        }

        bool pending_changes = false;
        // (a) check if any update exists since the last render...
        if( last_render_timestamp < last_change_timestamp ){
            auto render_age = clock::now() - last_render_timestamp;
            if( render_blackout < render_age ){
                pending_changes = true;
            }
        }

        last_render_timestamp = handler.update( pending_changes );
        sleep(0.01);
    }

    spdlog::info( cache.to_string());

    return EXIT_SUCCESS;
}

