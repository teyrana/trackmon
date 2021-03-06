#pragma once

#include <string>
#include <tuple>

// needed for certain handles, in class properties
#include <pcap/pcap.h>

#include "frame-buffer.hpp"

namespace readers {
namespace pcap {

/// \brief binary connector that reads .pcap (packet capture) files
///
/// References:
///   - https://www.tcpdump.org/manpages/pcap.3pcap.html
class LogReader {
public:

    LogReader( const std::string& filename );
    
    bool good() const;

    uint32_t length() const;

    /// \return true on success; false on failure
    bool open( const std::string& filename );

    /// \brief returns the next network frame
    /// \return get a pointer to a valid libpcap packet entry, or `nullptr` on error or EOF
    const FrameBuffer& next();

    uint64_t timestamp() const;

    bool set_filter_udp();

    bool set_filter_tcp();

    bool set_filter_port( uint16_t next_port);

private:
    int layer_2_protocol_;
    bool eof;

    // filter criteria
    uint8_t filter_layer_4_proto;
    uint16_t filter_layer_4_port;

    pcap_t* pcap_handle_;

    FrameBuffer cache;

};

}
}
