#include <iostream>
#include <string>
#include <vector>

#ifdef ENABLE_MOOS
#include "MBUtils.h"
#endif

// #include "track-monitor.hpp"
#include "core/track-cache.hpp"
#include "connectors/file/pcap-file-connector.hpp"
#include "connectors/file/text-file-connector.hpp"
#include "parsers/ais/ais-parser.hpp"

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
    for(int i=1; i<argc; i++) {
        std::string argi = argv[i];

        if((argi == "-h") || (argi == "--help") || (argi=="-help")){
            print_help();
            return 0;
        }else if((argi == "-v") || (argi == "--version")){
            std::cout << binary_name << "    Version: " << binary_version << std::endl;
            return 0;
        }
        // add more arguments here.

#ifdef ENABLE_MOOS
        }else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++")){
            mission_file = argi;
        }else if(i==2){
            run_command = argi;
        }

        if(mission_file == ""){
            showHelpAndExit();
        }
#endif
    }


#ifdef ENABLE_MOOS
    TrackMonitor monitor; 
    monitor.Run(run_command.c_str(), mission_file.c_str());
    fprintf(stderr, "MOOS Client Finished.\n");
#endif

    // ===========================================================================================
    std::cout << ">>> .A. Creating Track Database:" << std::endl;
    TrackCache cache;

    // ===========================================================================================
    std::cout << ">>> .B. Creating Connectors:" << std::endl;
    
    // const std::string ais_file = "data/ais.nmea0183.2022-05-18.log";
    // const std::string ais_file = "data/ais.nmea0183.2022-05-19.log";
    const std::string ais_file = "data/ais.tcpdump.2022-05-18.pcap";

    std::cout << "    >> Creating File Connector to:" << ais_file << std::endl;
    //TextFileConnector conn( ais_file );
    PCAPFileConnector conn( ais_file );

    if( ! (conn.good()) ){
        std::cerr << "!!! Could not create all connectors\n";
        return EXIT_FAILURE;
    }

    // ===========================================================================================
    std::cout << ">>> .C. Creating Parsers:" << std::endl;
    std::cout << "    >> Creating AIS Parser..." << std::endl;

    AISParser parser;

    // ===========================================================================================
    std::cout << ">>> .D. Starting Main-Loop:\n"
              << "    ...." << std::endl;

    uint32_t iteration_number = 0;
    const uint32_t iteration_limit = 0;  // 0 => disable limit
    uint32_t update_count = 0;
    while( (0 == iteration_limit) || (iteration_limit < iteration_number) ){

        // .1. get next data chunk
        const auto chunk = conn.next();
        if( 0 == chunk.length ){
            std::cout << "    (EOF)" << std::endl;
            break;
            // continue;
        }

        // .2. Load next chunk into parser
        parser.load( chunk.timestamp, chunk.buffer, chunk.length );

        // .3. Pull reports out of parser until drained
        const Report* report = parser.parse();
        while( report ){
            // Update cache
            cache.update( *report );
            ++update_count;

            // Pull next report
            report = parser.parse();
        }

        ++iteration_number;
    }
    std::cout << "<<< .E. Finished Ingesting; Found " << update_count << " updates." << std::endl;

    std::cout << cache.to_string() << std::endl;

    return EXIT_SUCCESS;
}
