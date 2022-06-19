#include <array>
#include <filesystem>
#include <iostream>

#include <pcap/pcap.h>

#include "pcap-log-reader.hpp"

static char error_message_buffer[PCAP_ERRBUF_SIZE];

namespace readers {
namespace NMEA0183 {

PCAPLogReader::PCAPLogReader( const std::string& filename )
    : pcap_handle_(nullptr)
{
    pcap_init(PCAP_CHAR_ENC_UTF_8, error_message_buffer);

    open( filename );
}

bool PCAPLogReader::good() const {
    return (nullptr != pcap_handle_);
}

uint32_t PCAPLogReader::length() const {
    return this->frame_header_.len;
}


bool PCAPLogReader::open( const std::string& filename ){
    if( ! std::filesystem::exists(std::filesystem::path(filename))){
        std::cerr << "?!? file is missing: " << filename << '\n';
        std::cerr << "::cwd: " << std::filesystem::current_path().string() << '\n';
        return false;
    }

    pcap_handle_ = pcap_open_offline( filename.c_str(), error_message_buffer);

    return good();
}

const PCAPLogReader::FrameBuffer& PCAPLogReader::next() {
    constexpr size_t udp_payload_offset = 42;

    auto* packet = pcap_next( pcap_handle_, &frame_header_ );
    if( (nullptr != packet) && (0x45 == packet[14]) ){
        cache.timestamp = timestamp();
        cache.buffer = const_cast<uint8_t*>(packet + udp_payload_offset);
        // const_cast<uint8_t*>(packet + udp_payload_offset),
        cache.length = frame_header_.len - udp_payload_offset;
        return cache;

    }else{
        // EOF
        cache.length = 0;
        return cache;
    }
}

uint64_t PCAPLogReader::timestamp() const {
    const struct timeval& ts = this->frame_header_.ts;
    return (ts.tv_sec*1'000'000 + ts.tv_usec);
}

}  // namespace NMEA0183
}  // namespace readers

