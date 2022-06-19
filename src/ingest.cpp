// Standard Library Includes
#include <iostream>
#include <string>
#include <vector>

// Dependency Includes
#include <cxxopts.hpp>
#include <fmt/core.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Project Includes
#include "core/track-cache.hpp"
#include "readers/pcap/log-reader.hpp"
// #include "readers/nmea0183/text-log-reader.hpp"
#include "parsers/ais/parser.hpp"
#include "parsers/moos/message-parser.hpp"
#include "parsers/moos/packet-parser.hpp"
#include "parsers/nmea0183/packet-parser.hpp"

const static std::string binary_name = "trackmon";
const static std::string binary_version = "0.0.1";


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
    cxxopts::Options options("trackgest", "ingest some tracks, and debug the result");
    options.add_options()
        ("b,build", "Display Build Information")
        ("l,limit", "limit processing to this many packets.  0 (default) processes all traffic.", cxxopts::value<int>()->default_value("0"))
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

    // DEBUG 
    // cache.set_origin( 29.712372, -91.880144 );  // Origin for AIS Data

    // cache.set_origin(42.357591,-71.082075);  // middle of Charles River Basin
    // cache.transform_to_utm(true);

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

    spdlog::info("    >> Creating File Connector to: {}", input_pcap_file);
    readers::pcap::LogReader reader( input_pcap_file );
    reader.set_filter_tcp();

    // // all of these exist, but I'm not sure which I care about, yet... TBD
    reader.set_filter_port(9000);

    if( ! (reader.good()) ){
        spdlog::error( "!!! Could not create all connectors" );
        return EXIT_FAILURE;
    }

    // ===========================================================================================
    spdlog::info(">>> .C. Creating Parsers:");
#ifdef ENABLE_AIS
    spdlog::info("    >> Creating AIS Parser...");
    parsers::NMEA0183::NMEA0183PacketParser nmea_parser;
    parsers::AIS::AISParser ais_parser;
#endif

#ifdef ENABLE_MOOS
    spdlog::info( "    >> Creating MOOS Parser..." );
    parsers::moos::PacketParser moos_packet_parser;
    spdlog::info( "    >> Creating Node-Report Parser..." );
    parsers::moos::MessageParser moos_report_parser;
#endif

    // ===========================================================================================
    spdlog::info(">>> .D. Ingest Updates:");

    uint32_t iteration_number = 0;
    const uint32_t iteration_limit = clargs["limit"].as<int>();
    uint32_t update_count = 0;
    while( (0 == iteration_limit) || ( iteration_number <= iteration_limit ) ){
        ++iteration_number;

        // spdlog::trace("    @ {:4d}\n", iteration_number );

        // .1. get next data chunk
        const auto chunk = reader.next();
        if( 0 == chunk.length ){
            if( not reader.good() ){
                spdlog::warn("    <<< @{:4d} -- EOF", iteration_number );
                break;
            } 
            // spdlog::trace( "    <<< @{:4d} -- Skip\n", iteration_number );
            continue;
        }
        // spdlog::debug( "    >>>> @{:4d}", iteration_number );

        // .2. Load next packet into parser
        moos_packet_parser.load( &chunk );
        
        // .3. Pull MOOS Messages out of packet; until empty
        while( ! moos_packet_parser.empty() ){
            const std::string line = moos_packet_parser.next();
            if( line.empty() ){
                // spdlog::trace("        <<<");
                continue;
            }

            // .3. Pull reports out of parser until drained
            Report* report = moos_report_parser.parse( line );
            if( report ){
                cache.update( *report );
                ++update_count;
            }
        }

        // // .2. Load next chunk into parser
        // // TODO: make this more functional
        // nmea_parser.load( &chunk );
        //
        // // .3. Pull NMEA-0183 lines out of chunk, until empty
        // while( ! nmea_parser.empty() ){
        //     const std::string line = nmea_parser.next();
        //     if( line.empty() ){
        //         continue;
        //     }
        //     // std::cerr << "        <<< line:    (" << line.size() << "): " << line << std::endl;
        //
        //     // .3. Pull reports out of parser until drained
        //     Report* report = ais_parser.parse( line );
        //     if( report ){
        //         report -> timestamp = chunk.timestamp
        //         cache.update( *report );
        //         ++update_count;
        //     }
        // }
    }
    spdlog::info("<<< .E. Finished Ingesting; Found {} updates.", update_count );

    spdlog::info(cache.to_string());

    return EXIT_SUCCESS;
}
