#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>

#include "linux/if_ether.h"
#include "linux/ip.h"
#include "linux/tcp.h"
#include "linux/udp.h"

#include <pcap/pcap.h>

#include "log-reader.hpp"

static char error_message_buffer[PCAP_ERRBUF_SIZE];

namespace readers {
namespace pcap {

static constexpr size_t ipv4_header_length = sizeof(struct iphdr);
static_assert( ipv4_header_length == 20, "IP Header size does not match??");
static constexpr size_t udp_header_length = sizeof(struct udphdr);
static_assert( udp_header_length == 8, "UDP Header size does not match??");


LogReader::LogReader( const std::string& filename )
    : datalink_layer_type_(DLT_NULL)
    , eof(true)
    , filter_layer_4_proto(0)
    , filter_layer_4_port(0)
    , pcap_handle_(nullptr)
{
    pcap_init(PCAP_CHAR_ENC_UTF_8, error_message_buffer);

    open( filename );
}

bool LogReader::good() const {
    return ( (nullptr!=pcap_handle_) && (!eof) );
}

uint32_t LogReader::length() const {
    return this->cache.length;
}

bool LogReader::open( const std::string& filename ){
    if( ! std::filesystem::exists(std::filesystem::path(filename))){
        std::cerr << "?!? file is missing: " << filename << '\n';
        std::cerr << "::cwd: " << std::filesystem::current_path().string() << '\n';
        return false;
    }

    pcap_handle_ = pcap_open_offline( filename.c_str(), error_message_buffer);
    if( nullptr != pcap_handle_ ){
      eof = false;
      datalink_layer_type_ = pcap_datalink(pcap_handle_);
      return true;
    }
    
    return false;
}

const FrameBuffer& LogReader::next() {
    pcap_pkthdr * frame_header;
    const uint8_t * read_buffer;
    const int result = pcap_next_ex( pcap_handle_, &frame_header, &read_buffer );
    if( 1 == result ){
        // success
    }else if( -2 == result ){
        // End-Of-File (EOF): No more packets
        eof = true;
        cache.length = 0;
        return cache;
    } else {
        pcap_perror( pcap_handle_, "" ); ///< print error to stderr
        cache.length = 0;
        return cache;
    }

    //std::cerr << "    :DATALINK-LAYER-TYPE: " << datalink_layer_type_ << std::endl;
    size_t ipv4_header_offset = ETH_HLEN;
    if( DLT_EN10MB == datalink_layer_type_ ){
        ipv4_header_offset = ETH_HLEN;    ///< from "linux/if_ether.h"
    }else if( DLT_LINUX_SLL == datalink_layer_type_ ){
        ipv4_header_offset = 16; // obtained by inspection [citation needed]
    }else{
        assert( false && "Path not implemented for Datalink-Layer!?");
    }
    assert( (0x45 == read_buffer[ipv4_header_offset]) && ("IPv4 Header was not in the expected location!?") );
      
    const uint8_t layer_4_proto = reinterpret_cast<const iphdr*>(read_buffer+ipv4_header_offset)->protocol;
    if( layer_4_proto == filter_layer_4_proto ){
        if( IPPROTO_TCP == layer_4_proto ){
            std::cerr << "    >>> Processing IPPROTO_TCP: " << IPPROTO_TCP << std::endl;
            // if( 0x45 == read_buffer[ipv4_header_offset] ){
            // NYI
            std::cerr << "    <<< ERROR!\n";
            // EOF
            cache.length = 0;
            return cache;
        }else if( IPPROTO_UDP == layer_4_proto ){
            const struct timeval& ts = frame_header->ts;
            cache.timestamp = (ts.tv_sec*1'000'000 + ts.tv_usec);
        
            const auto udp_header_offset = ipv4_header_offset + ipv4_header_length;
            // const udphdr * udp_header =  reinterpret_cast<const udphdr*>(read_buffer+udp_header_offset);
            const uint16_t udp_dest_port = ntohs(reinterpret_cast<const udphdr*>(read_buffer+udp_header_offset)->dest);

            if( filter_layer_4_port == udp_dest_port ){
                const size_t data_offset = ipv4_header_offset + ipv4_header_length + udp_header_length;
                cache.length = frame_header->len - data_offset;
                cache.buffer = const_cast<uint8_t*>(read_buffer + data_offset);
                return cache;
            }else{
                std::cerr << "    !?!? Port mismatch on UDP packet! "
                          << "    filter_port == " << filter_layer_4_port 
                          << "    udp_dest_port == " << udp_dest_port << std::endl;
            }
        }
    }else{
        std::cerr << "    !?!? Layer-4-Protocol mismatch!! "
                  << "    expected protocol: " << filter_layer_4_proto 
                  << "    found protocol: " << layer_4_proto << std::endl;
    }

    cache.length = 0;
    return cache;
}

bool LogReader::set_filter_tcp(){
    filter_layer_4_proto = IPPROTO_TCP;
    return true;
}

bool LogReader::set_filter_udp(){
    filter_layer_4_proto = IPPROTO_UDP;
    return true;
}

bool LogReader::set_filter_port( uint16_t next_filter_port ){
    filter_layer_4_port = next_filter_port;
    return true;
}

uint64_t LogReader::timestamp() const {
  return cache.timestamp;
}

}  // namespace pcap
}  // namespace readers

