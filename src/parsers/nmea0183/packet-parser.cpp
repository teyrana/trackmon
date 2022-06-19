#include <array>
#include <filesystem>
#include <iostream>


#include "packet-parser.hpp"

namespace parsers {
namespace nmea0183 {

bool PacketParser::empty() const {
    return (   (nullptr==cache)
            || (nullptr==cursor)
            || (cache->length <= static_cast<size_t>(cursor - cache->buffer)) 
    );
}

const uint8_t* PacketParser::find_byte( const uint8_t* find_start, uint8_t value ){
    const uint8_t* find_end = cache->buffer + cache->length;
    for( const uint8_t* cur = find_start; cur < find_end; ++cur ){
        if( value == *cur ){
            return cur;
        }
    }

    return nullptr;
}

uint32_t PacketParser::length() const {
    if( cache ){
        return cache->length;
    }
    return 0;
}

bool PacketParser::load( const readers::pcap::FrameBuffer* source ){
    cache = source;
    cursor = source->buffer;
    return true;
}


std::string PacketParser::next() {
    if( cache && cursor ){
        // std::cerr << "        :parser:at:        " << static_cast<int>(cursor - cache->buffer) << std::endl;
        // std::cerr << "        :buffer.timestamp: " << cache->timestamp << std::endl;
        // std::cerr << "        :buffer.length:    " << cache->length << std::endl;
        // std::string debug_buffer( reinterpret_cast<const char*>(cache->buffer), cache->length );
        // std::cerr << "        :buffer:buffer:    " << debug_buffer << "\n";

        const uint8_t* line_start = find_byte(cursor,  '!');
        if( nullptr == line_start){
            cursor = nullptr;
            return "";
        }

        const uint8_t* line_end = find_byte(line_start, '\r');
        if( nullptr == line_end){
            cursor = nullptr;
            return "";
        }

        const size_t line_length = (line_end - line_start);

        if( line_start && line_end && (12 < line_length) ){
            cursor += line_length + 2;
            return std::string( reinterpret_cast<const char*>(line_start), line_length );
        }
    }

    cursor = nullptr;
    return "";
}

uint64_t PacketParser::timestamp() const {
    return (nullptr==cache) ? 0 : cache->timestamp;
}

}  // namespace NMEA0183
}  // namespace parsers

