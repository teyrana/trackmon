// Standard Library Includes
#include <chrono>
#include <iostream>
#include <string>
#include <deque>
#include <vector>

// Dependency Includes
#include <cxxopts.hpp>
#include <fmt/core.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// Project Includes
#include "core/track-cache.hpp"
#include "readers/pcap/file-reader.hpp"

#ifdef ENABLE_AIS
#include "parsers/ais/parser.hpp"
#include "parsers/nmea0183/packet-parser.hpp"
#endif

#ifdef ENABLE_MOOS
#include "parsers/moos/message-parser.hpp"
#include "parsers/moos/packet-parser.hpp"
#endif

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
    std::deque<const core::ForwardBuffer*> packets;

#ifdef ENABLE_AIS
    const std::string input_pcap_file = "data/ais.tcpdump.2022-05-18.pcap";
#endif

#ifdef ENABLE_MOOS
    // const std::string input_pcap_file = "data/m2_berta.moos.pcap";
    // const std::string input_pcap_file = "data/m2_berta.moos.gt140.pcap";
    const std::string input_pcap_file = "data/m2_berta.moos.p9000.pcap";
#endif

    spdlog::info("    >> Creating File Connector to: {}", input_pcap_file);
    readers::pcap::FileReader reader( input_pcap_file );

#ifdef ENABLE_AIS
    reader.set_filter_udp();
    reader.set_filter_port(4003);
#endif 
#ifdef ENABLE_MOOS
    reader.set_filter_tcp();
    reader.set_filter_port(9000);
#endif

    if( ! reader.good() ){
        spdlog::error( "!!! Could not create all connectors" );
        return EXIT_FAILURE;
    }

    // ===========================================================================================
    spdlog::info(">>> .C. Creating Parsers:");
    std::deque<const core::StringBuffer*> messages;

#ifdef ENABLE_AIS
    spdlog::info("    >> Creating AIS Parser...");
    parsers::nmea0183::PacketParser nmea_packet_parser;
    parsers::ais::Parser ais_message_parser;
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
        const core::ForwardBuffer* chunk = reader.next();
        if( (nullptr != chunk) && (0 < chunk->length) ){
            packets.push_back(chunk);
        }

        // simple termination condition -- expand if we ever want multiple readers at once
        if( not reader.good() ){
            spdlog::warn("    <<< @{:4d} -- EOF", iteration_number );
            break;
        }

        // .2. convert data buffers to discrete packets
        while( 0 < packets.size() ){
            // .2.a. Pop each packet from queue of available packets
            const auto* each_packet = packets.front();
            packets.pop_front();

            // vvvv DEBUG
            // if( 1500 < chunk.length ){
            //     spdlog::warn("    length greater than MTU=={}    @{:4d}", chunk.length, iteration_number );
            // }
            // ^^^^ DEBUG

#ifdef ENABLE_AIS
            // .2.b. Load packet into parser and pull messages until empty
            // .2.c. Pull messages out of parser; until empty
            nmea_packet_parser.load( each_packet );
            while( ! nmea_packet_parser.empty() ){
                const auto* line = nmea_packet_parser.next();
                if(line){
                    messages.push_back(line);
                }
            }
#endif
#ifdef ENABLE_MOOS
            // .2.b. Load packet into parser and pull messages until empty
            // .2.c. Pull messages out of parser; until empty
            moos_packet_parser.load( each_packet );
            while( ! moos_packet_parser.empty() ){
                const auto* line = moos_packet_parser.next();
                if(line){
                    messages.push_back(line);
                }
            }
#endif
        }

        // .3. convert data buffers to discrete packets
        while( 0 < messages.size() ){
            // .3.a. Pop each packet from the queue of available messages
            const core::StringBuffer& next_message = *messages.front();
            messages.pop_front();

            // spdlog::debug( "        ##: {}", *each_line );

            // .3.b. Pull reports out of parser until drained
#ifdef ENABLE_AIS
            Report* report = ais_message_parser.parse( next_message );
#endif
#ifdef ENABLE_MOOS
            Report* report = moos_report_parser.parse( next_message );
#endif

            if( report ){
                cache.update( report );
                ++update_count;
            }
        }

        // {
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
    
    }
    spdlog::info("<<< .E. Finished Ingesting; Found {} updates.", update_count );

    spdlog::info(cache.to_string());

    return EXIT_SUCCESS;
}
