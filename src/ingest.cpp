#include <iostream>
#include <string>
#include <vector>

#include <cxxopts.hpp>

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

void print_help(){
    std::cout << "\n"
        << "====== ====== " << binary_name << " ====== ======\n"
        << "Synopsis\n"
        << "-------------------------------------\n"
        << "  This app monitors track traffic, and provides a dynamic, configurable way to inspect same.\n"
        << "  In-app commands are displayed in the screen footer.\n"
        << "\n"
        << "Options:\n"
        << "  <None>\n"
        << "  --version,-v\n"
        << "    Display the release version of this binary.\n"
        << "\n";
}

int main(int argc, char *argv[]){

    // Create a cxxopts::Options instance.
    cxxopts::Options options("trackgest", "ingest some tracks, and debug the result");
    options.add_options()
        // ("f,foo", "Param foo", cxxopts::value<int>()->default_value("10"))
        ("l,limit", "limit processing to this many packets.  0 (default) processes all traffic.", cxxopts::value<int>()->default_value("0"))
        ("h,help", "Print usage")
        ("v,verbose", "Verbose output")
        ("V,Version", "Print Version")
    ;
    const auto clargs = options.parse(argc, argv);

    if( clargs["Version"].as<bool>() ){
        std::cout << binary_name << "    Version: " << "0.0.1-beta" << std::endl;
        exit(0);
    }else if (clargs["help"].as<bool>() ){
        std::cout << options.help() << std::endl;
        exit(0);
    }

    std::cerr << "::verbosity = " << clargs["verbose"].count() << std::endl;
    // std::cerr << "::verbosity = " << clargs["verbose"].as<int>() << std::endl;

    // ===========================================================================================
    std::cout << ">>> .A. Creating Track Database:" << std::endl;
    TrackCache cache;

    // DEBUG 
    cache.set_origin( 29.712372, -91.880144 );  // Origin for AIS Data
    // cache.set_origin(42.357591,-71.082075);  // middle of Charles River Basin
    // cache.transform_to_utm(true);

    // ===========================================================================================
    std::cout << ">>> .B. Creating Connectors:" << std::endl;
    
    // // const std::string ais_file = "data/ais.nmea0183.2022-05-18.log";
    // // const std::string ais_file = "data/ais.nmea0183.2022-05-19.log";
    const std::string input_pcap_file = "data/ais.tcpdump.2022-05-18.pcap";
    // const std::string input_pcap_file = "data/m2_berta.moos.pcap";

    std::cout << "    >> Creating File Connector to:" << input_pcap_file << std::endl;
    readers::pcap::LogReader reader( input_pcap_file );
    reader.set_filter_udp();
    reader.set_filter_port(4003);

    if( ! (reader.good()) ){
        std::cerr << "!!! Could not create all connectors\n";
        return EXIT_FAILURE;
    }

    // ===========================================================================================
    std::cout << ">>> .C. Creating Parsers:" << std::endl;
    std::cout << "    >> Creating AIS Parser..." << std::endl;
    parsers::nmea0183::PacketParser nmea_parser;
    parsers::ais::Parser ais_parser;

    // ===========================================================================================
    std::cout << ">>> .D. Ingest Updates:\n"
              << "    ...." << std::endl;

    uint32_t iteration_number = 0;
    const uint32_t iteration_limit = clargs["limit"].as<int>();
    uint32_t update_count = 0;
    while( (0 == iteration_limit) || ( iteration_number < iteration_limit ) ){
        // fprintf( stderr, "    @ %03u\n", iteration_number );
        
        // .1. get next data chunk
        const auto chunk = reader.next();
        if( 0 == chunk.length ){
            if( not reader.good() ){
                std::cout << "    <<< EOF" << std::endl;
                break;
            } 
            std::cout << "    <read failure>" << std::endl;
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
            // std::cerr << "        <<< line:    (" << line.size() << "): " << line << std::endl;

            // .3. Pull reports out of parser until drained
            Report* report = ais_parser.parse( chunk.timestamp, line );
            if( report ){
                cache.update( *report );
                ++update_count;
            }
        }

        ++iteration_number;
    }
    std::cout << "<<< .E. Finished Ingesting; Found " << update_count << " updates." << std::endl;

    std::cout << cache.to_string() << std::endl;

    return EXIT_SUCCESS;
}
