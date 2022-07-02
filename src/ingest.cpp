#include <iostream>
#include <string>
#include <vector>

#include <cxxopts.hpp>
#include <fmt/core.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// 1st Party includes
#include "core/track-cache.hpp"
#include "readers/pcap/log-reader.hpp"
// #include "readers/nmea0183/text-log-reader.hpp"
#include "parsers/ais/parser.hpp"
// #include "parsers/moos/moos-node-report-parser.hpp"
// #include "readers/moos/moos-packet-reader.hpp"
#include "parsers/nmea0183/packet-parser.hpp"

const static std::string binary_name = "trackmon";
const static std::string binary_version = "0.0.1";

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

    // DEBUG 
    cache.set_origin( 29.712372, -91.880144 );  // Origin for AIS Data
    // cache.set_origin(42.357591,-71.082075);  // middle of Charles River Basin
    // cache.transform_to_utm(true);

    // ===========================================================================================
    spdlog::info(">>> .B. Creating Connectors:");
    
    // // const std::string ais_file = "data/ais.nmea0183.2022-05-18.log";
    // // const std::string ais_file = "data/ais.nmea0183.2022-05-19.log";
    const std::string input_pcap_file = "data/ais.tcpdump.2022-05-18.pcap";
    // const std::string input_pcap_file = "data/m2_berta.moos.pcap";

    spdlog::info("    >> Creating File Connector to: {}", input_pcap_file);
    readers::pcap::LogReader reader( input_pcap_file );
    reader.set_filter_udp();
    reader.set_filter_port(4003);

    if( ! (reader.good()) ){
        spdlog::error( "!!! Could not create all connectors" );
        return EXIT_FAILURE;
    }

    // ===========================================================================================
    spdlog::info(">>> .C. Creating Parsers:");
    spdlog::info("    >> Creating AIS Parser...");
    parsers::nmea0183::PacketParser nmea_parser;
    parsers::ais::Parser ais_parser;

    // ===========================================================================================
    spdlog::info(">>> .D. Ingest Updates:");
    spdlog::debug("    ....");

    uint32_t iteration_number = 0;
    const uint32_t iteration_limit = clargs["limit"].as<int>();
    uint32_t update_count = 0;
    while( (0 == iteration_limit) || ( iteration_number < iteration_limit ) ){
        // fprintf( stderr, "    @ %03u\n", iteration_number );
        
        // .1. get next data chunk
        const auto chunk = reader.next();
        if( 0 == chunk.length ){
            if( not reader.good() ){
                spdlog::debug("    <<< EOF");
                break;
            } 
            spdlog::debug("    <read failure>");
            continue;
        }

        // .2. Load next chunk into parser
        // TODO: make this more functional
        nmea_parser.load( &chunk );

        // .3. Pull NMEA-0183 lines out of chunk, until empty
        while( ! nmea_parser.empty() ){
            const std::string line = nmea_parser.next();
            if( line.empty() ){
                break;
            }
            // spdlog::debug( "        <<< line:    ({}): {} ", line.size(), line );

            // .3. Pull reports out of parser until drained
            Report* report = ais_parser.parse( chunk.timestamp, line );
            if( report ){
                cache.update( *report );
                ++update_count;
            }
        }

        ++iteration_number;
    }
    spdlog::info("<<< .E. Finished Ingesting; Found {} updates.", update_count );

    spdlog::info(cache.to_string());

    return EXIT_SUCCESS;
}
