// std library includes
#include <array>
#include <filesystem>
#include <iostream>

// 3rd party includes
#include <ais.h>

// 1st party includes
#include "ais-parser.hpp"


// vvvv monkey-patch
// function is technically exported from the upstream library, but isn't included in the ais.h header.
namespace libais {
std::unique_ptr<AisMsg> CreateAisMsg(const string &body, const int fill_bits);
}  // namespace libais
// ^^^^ monkey-patch

namespace parsers {
namespace AIS {

// ===============================================================================

// uint8_t* find_byte( uint8_t* start, uint8_t* end, const uint8_t value ){
//     for( const uint8_t* cur = start; cur < end; ++cur ){
// 	if( value == *cur ){
// 	    return cur;
// 	}
//     }
//     return nullptr;
// }

const uint8_t* find_byte( const uint8_t* start, const uint8_t* end, const uint8_t value ){
    for( const uint8_t* cur = start; cur < end; ++cur ){
        if( value == *cur ){
            return cur;
        }
    }
    return nullptr;
}

void print_bytes( const char* prefix, const uint8_t* data ) {
    printf( "%s  %02X%02X%02X%02X  %02X%02X%02X%02X", prefix, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7] );
}

bool AISParser::load( uint64_t timestamp, uint8_t* _buffer, size_t length ) {

    timestamp_ = timestamp;
    data_start_ = _buffer;
    data_cursor_ = data_start_;
    data_end_ = data_start_ + length;

    // DEBUG
    // fprintf( stderr, ">>> Loaded @%16lu.%06lu |%lu|\n", timestamp_/1'000'000, timestamp_%1'000'000, std::get<2>(chunk) );

    return true;
}

Report* AISParser::parse() {

    // note: the early hot-path-early-exit
    while( data_cursor_ < data_end_ ){
        const uint8_t * line_start = find_byte( data_cursor_, data_end_, '!');
        if( nullptr == line_start ){
            break;
        }

        const uint8_t * line_end = find_byte(line_start, data_end_, '\r');
        if( nullptr == line_end ){
            break;
        }

        // Ignore sentence if it's too short to contain data
        if( (line_end - line_start) < 12 ){
            std::cerr << "!!!! line is too short to process!!\n";
            return nullptr;
        }
        
        // .2.A. Process all sentences from this packet
        const std::string line( reinterpret_cast<const char*>(line_start), (line_end - line_start) );
            
        data_cursor_ = ((uint8_t*)line_end + 2);
        
        // // .3.B. Pass sentence to nmea parser
        export_.timestamp = timestamp_;
        return parse_nmea_sentence( line );
    }

    data_cursor_ = data_end_;
    return nullptr;
}

Report* AISParser::parse_nmea_sentence( const std::string& sentence ){

    // const bool is_ownship = ('O'==line.at(5)); // correct, but unused
    // const char ownship_flag = sentence.at(5);

    // i.e. this message is `fragment_number` of `fragment_count` total fragments
    const uint8_t fragment_count = sentence.at(7) - '0';
    const uint8_t fragment_total = sentence.at(9) - '0';
    // used to associate fragment streams
    //const uint8_t fragment_stream = line.at(11) - '0';  // correct? ; unused

    // TODO: Implement a fragment buffer
    if( 1 < fragment_count ){
        std::cerr << "!?!? multi-fragment messages are not-yet-supported.\n"
                  << "     << " << sentence << std::endl;
        return nullptr;
    }

    //fprintf( stderr, "    >>Extracting: _%s_\n", line.c_str() );

    const std::string body = libais::GetBody( sentence );

    const size_t checksum_index = sentence.find('*') + 1;
    const size_t padding_index = checksum_index - 2;

    // TODO: check checksum....

    if( std::string::npos == padding_index ){
        std::cerr << "!?!? NMEA 0183 sentence did not contain a trailing '*' -- parse error!" << std::endl;
        return nullptr;
    }
    const uint8_t pad_bit_count = sentence.at(checksum_index-2) - '0';

    const auto msg = ::libais::CreateAisMsg( body, pad_bit_count);
    if( msg->had_error() ){
        std::cerr << "?ERROR: " << msg->get_error() << "): " << libais::AIS_STATUS_STRINGS[msg->get_error()] << std::endl;
        return nullptr;
    }
    
    export_.name.clear();
    export_.source = Report::AIS;

    // get track from db
    // auto report => db.get_track(msg->mmsi);
    switch(msg->message_id){
        case 1:
        case 2: 
        case 3:{
            fprintf(stdout, "    >> NYI: [%hu](# %u)\n", msg->message_id, msg->mmsi );
            //const auto* msg1 = reinterpret_cast<libais::Ais1_2_3*>(msg.get());
            break;}
        case 4:{ // 4 bsreport and 11 utc date response
            // Ignoring this message; we're not interested in this information.
            // fprintf(stdout, "    >> NYI: [%hu](# %u)\n", msg->message_id, msg->mmsi );
            // const auto* msg4 = reinterpret_cast<libais::Ais4_11*>(msg.get());
            return nullptr;}
        case 18:{ // 18 - 'B' - Class B position report
            export_.id = msg->mmsi;
            const auto* msg18 = reinterpret_cast<libais::Ais18*>(msg.get());
            // fprintf(stdout, "    >> processing: [%hu](# %u)\n", msg->message_id, msg->mmsi );
            export_.speed = msg18->sog;
            export_.status = 15;  // 'undefined' status, according to AIS: 
            export_.longitude = msg18->position.lng_deg;
            export_.latitude = msg18->position.lat_deg;
            export_.course = msg18->cog;
            export_.heading = msg18->true_heading;
            break;}
        case 24:{  // 24 - 'H' - Class B Static Data report
            // Ignoring this message; we're not interested in this information.
            // fprintf(stdout, "    >> Processing:24:/B Static Data: [%hu](# %u)\n", msg->message_id, msg->mmsi );
            // export_.id = msg->mmsi;
            // const auto* msg24 = reinterpret_cast<libais::Ais24*>(msg.get());
            return nullptr;}
        case 27:{  // 27 - 'K' - Long-range position report - e.g. for satellite receivers
            export_.id = msg->mmsi;
            const auto* msg27 = reinterpret_cast<libais::Ais27*>(msg.get());
            export_.status = msg27->nav_status;
            export_.speed = msg27->sog;
            export_.longitude = msg27->position.lng_deg;
            export_.latitude = msg27->position.lat_deg;
            export_.course = msg27->cog;
            export_.heading = NAN;

            // // unhandled fields:
            // int position_accuracy;
            // bool raim;

            break;}
        default:
            std::cerr << "XX: un-implemented message type!!:    <" << static_cast<int>(msg->message_id) << ">:mmsi: " << msg->mmsi << std::endl;
            std::cerr << "        ...:   " << sentence << '\n';
            if( 1 < fragment_total ){
                std::cerr << "        ...pt: " << static_cast<int>(fragment_count) << " / " << static_cast<int>(fragment_total) << '\n';
            }

        return nullptr;
    }

    return &export_;
}


}  // namespace AIS
}  // namespace parsers