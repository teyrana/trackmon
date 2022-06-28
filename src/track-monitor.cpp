#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <ncurses.h>

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
    }

    // ===========================================================================================
    std::cout << ">>> .A. Creating Track Database:" << std::endl;
    TrackCache cache;

    // ===========================================================================================
    std::cout << ">>> .B. Creating Connectors:" << std::endl;
    
    // const std::string ais_file = "data/ais.nmea0183.2022-05-18.log";
    // const std::string ais_file = "data/ais.nmea0183.2022-05-19.log";
    // std::cout << "    :> Creating File Connector to:" << ais_file << std::endl;
    // TextLogReader reader( ais_file );

    const std::string ais_file = "data/ais.tcpdump.2022-05-18.pcap";
    std::cout << "    :> Creating File Connector to:" << ais_file << std::endl;
    readers::pcap::LogReader reader( ais_file );
    reader.set_filter_udp();
    reader.set_filter_port(4003);
    if( ! (reader.good()) ){
        std::cerr << "!!! Could not create all connectors\n";
        return EXIT_FAILURE;
    }

    // ===========================================================================================
    std::cout << ">>> .C. Creating Parsers:" << std::endl;
    std::cout << "    :> Creating AIS Parser..." << std::endl;
    parsers::nmea0183::PacketParser nmea_parser;
    parsers::ais::Parser ais_parser;

    // ===========================================================================================
    std::cout << ">>> .D. Building UI: " << std::endl;
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

    std::cout << cache.to_string() << std::endl;

    return EXIT_SUCCESS;
}

