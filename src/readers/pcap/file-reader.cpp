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
#include <spdlog/spdlog.h>

#include "file-reader.hpp"

static char error_message_buffer[PCAP_ERRBUF_SIZE];

namespace readers {
namespace pcap {

static constexpr size_t ipv4_header_length = sizeof(struct iphdr);
static_assert( ipv4_header_length == 20, "IP Header size does not match??");
static constexpr size_t udp_header_length = sizeof(struct udphdr);
static_assert( udp_header_length == 8, "UDP Header size does not match??");


FileReader::FileReader( const std::string& filename )
    : layer_2_protocol_(DLT_NULL)
    , eof(true)
    , filter_layer_4_proto(0)
    , filter_layer_4_port(0)
    , pcap_handle_(nullptr)
{
    pcap_init(PCAP_CHAR_ENC_UTF_8, error_message_buffer);

    open( filename );
}

bool FileReader::good() const {
    return ( (nullptr!=pcap_handle_) && (!eof) );
}

uint32_t FileReader::length() const {
    return this->cache.length;
}

bool FileReader::open( const std::string& filename ){
    if( ! std::filesystem::exists(std::filesystem::path(filename))){
        std::cerr << "?!? file is missing: " << filename << '\n';
        std::cerr << "::cwd: " << std::filesystem::current_path().string() << '\n';
        return false;
    }

    pcap_handle_ = pcap_open_offline( filename.c_str(), error_message_buffer);
    if( nullptr != pcap_handle_ ){
      eof = false;
      layer_2_protocol_ = pcap_datalink(pcap_handle_);
      return true;
    }
    
    return false;
}

const core::ForwardBuffer* FileReader::next() {
    pcap_pkthdr * frame_header;
    const uint8_t * read_buffer;
    const int result = pcap_next_ex( pcap_handle_, &frame_header, &read_buffer );
    if( 1 == result ){
        // success
    }else if( -2 == result ){
        // End-Of-File (EOF): No more packets
        eof = true;
        return nullptr;
    } else {
        std::cerr << "    !!Unrecognized error from 'pcap_next_ex'...." << result << std::endl;
        pcap_perror( pcap_handle_, "" ); ///< print error to stderr
        cache.length = 0;
        return nullptr;
    }

    // std::cerr << "    ::DATALINK-LAYER-TYPE: " << layer_2_protocol_ << std::endl;
    size_t ipv4_header_offset = ETH_HLEN;
    uint16_t layer_3_protocol = 0;

    // check Layer 2 protocol:
    if( DLT_EN10MB == layer_2_protocol_ ){
        ipv4_header_offset = ETH_HLEN;    ///< from "linux/if_ether.h"
        layer_3_protocol = ntohs( * reinterpret_cast<const uint16_t*>(read_buffer + 12));
    }else if( DLT_LINUX_SLL == layer_2_protocol_ ){
        ipv4_header_offset = 16; // obtained by inspection [citation needed]
        layer_3_protocol = ntohs( * reinterpret_cast<const uint16_t*>(read_buffer + 14));
    }else{
        assert( false && "Path not implemented for Datalink-Layer!?");
    }

    // check Layer 3 protocol:
    switch(layer_3_protocol){
        case ETH_P_IP:{ // Internet Protocol packet
            // if this is an IPv4 packet we BETTER find an IPv4 marker:
            if( 0x45 != read_buffer[ipv4_header_offset]){
                std::cerr << "IPv4 Header was not in the expected location!?" << std::endl;
                cache.length = 0;
                return nullptr;
            }
            break;
        }
        case ETH_P_802_2:
        case ETH_P_ARP:
        case ETH_P_IPV6:
            // code does not handle these protocols. Ignore and return an error:
            cache.length = 0;
            return nullptr;
        default:
            fprintf( stderr, "<<!! Found Unknown Layer 3 Protocol: %ud == %04xh\n", layer_3_protocol, layer_3_protocol );
            fprintf( stderr, "<<!! Path not implemented for Layer 3 Protocol!!\n");
            assert( false );
    }

    const auto * const ip = reinterpret_cast<const iphdr*>(read_buffer+ipv4_header_offset);

    // vvvv DEBUG
    // const uint16_t layer_3_length = ntohs(ip->tot_len);
    // std::cerr << "        ::Layer-3-length: " << static_cast<int>(layer_3_length) << std::endl;

    // const uint16_t layer_4_length = layer_3_length - ipv4_header_length;
    // std::cerr << "        ::Layer-4-length: " << static_cast<int>(layer_4_length) << std::endl;
    // ^^^^ DEBUG

    // std::cerr << "        ::Layer-4-Protocol: " << static_cast<int>(layer_4_proto) << std::endl;
    const uint16_t layer_4_proto = ip->protocol;
    switch( layer_4_proto ){
        case IPPROTO_TCP:{
            // reference: https://en.wikipedia.org/wiki/Transmission_Control_Protocol
            if( layer_4_proto != filter_layer_4_proto ){
                break;
            } 

            const struct timeval& ts = frame_header->ts;
            cache.timestamp = (ts.tv_sec*1'000'000 + ts.tv_usec);

            const auto tcp_header_offset = ipv4_header_offset + ipv4_header_length;

            // extract destination port
            const uint16_t tcp_dest_port = ntohs(*reinterpret_cast<const uint16_t*>(read_buffer+tcp_header_offset + 2));
            // placeholder?  notify-only-filter
            if( filter_layer_4_port != tcp_dest_port ){
                spdlog::trace( "    !?!? Port mismatch on TCP packet!   found: {} =/= {} :filter", tcp_dest_port, filter_layer_4_port );
                break; // abort
            }

            // extract the "data offset" field from the tcp header, and perform the necessary transforms
            const uint8_t tcp_header_length =  4 * ( 0xF0 & *(read_buffer + tcp_header_offset + 12)) >> 4;
            const size_t payload_offset = tcp_header_offset + tcp_header_length;

            cache.length = frame_header->len - payload_offset;
            // fprintf( stderr, "        ::packet:|%lu|...\n", cache.length);
            cache.buffer = const_cast<uint8_t*>(read_buffer + payload_offset);
            // fprintf( stderr, "        ::pay:: [@ %2ld]: %02x %02x %02x %02x\n", payload_offset, payload[0], payload[1], payload[2], payload[3] );
            return &cache;
        }
        case IPPROTO_UDP:{
            // reference: https://en.wikipedia.org/wiki/User_Datagram_Protocol
            if( layer_4_proto != filter_layer_4_proto ){
                break;  // abort
            }

            const struct timeval& ts = frame_header->ts;
            cache.timestamp = (ts.tv_sec*1'000'000 + ts.tv_usec);

            const auto udp_header_offset = ipv4_header_offset + ipv4_header_length;
            // const udphdr * udp_header =  reinterpret_cast<const udphdr*>(read_buffer+udp_header_offset);
            const uint16_t udp_dest_port = ntohs(reinterpret_cast<const udphdr*>(read_buffer+udp_header_offset)->dest);

            if( filter_layer_4_port == udp_dest_port ){
                const size_t data_offset = ipv4_header_offset + ipv4_header_length + udp_header_length;
                cache.length = frame_header->len - data_offset;
                cache.buffer = const_cast<uint8_t*>(read_buffer + data_offset);
                return &cache;
            }else{
                spdlog::trace( "    !?!? Port mismatch on UDP packet!   found: {:d} =/= {:d} :filter", udp_dest_port, filter_layer_4_port );
                break; // abort
            }
        }
        case IPPROTO_ICMP:
        // case 128:
            // noop && ignore
            break;
        default:
            spdlog::trace("    !?!? Layer-4-Protocol mismatch!!  found: {:d} =/= {:d} :filter",  layer_4_proto, filter_layer_4_proto );
            break; // abort
    }

    // Failure Clean Up
    cache.length = 0;
    return nullptr;
}

bool FileReader::set_filter_tcp(){
    filter_layer_4_proto = IPPROTO_TCP;
    return true;
}

bool FileReader::set_filter_udp(){
    filter_layer_4_proto = IPPROTO_UDP;
    return true;
}

bool FileReader::set_filter_port( uint16_t next_filter_port ){
    filter_layer_4_port = next_filter_port;
    return true;
}

uint64_t FileReader::timestamp() const {
  return cache.timestamp;
}

}  // namespace pcap
}  // namespace readers

