#pragma once

#include <array>
#include <string>
#include <tuple>


#include <pcap/pcap.h>

/// \brief binary connector that reads .pcap (packet capture) files
///
/// References:
///   - https://www.tcpdump.org/manpages/pcap.3pcap.html
class PCAPFileConnector {
public:
    struct FrameBuffer {
        uint64_t timestamp;
        uint8_t* buffer;
        size_t length;
    };

    PCAPFileConnector( const std::string& filename );
    
    bool good() const;

    uint32_t length() const;

    /// \return true on success; false on failure
    bool open( const std::string& filename );

    /// \brief returns the next network frame
    /// \return get a pointer to a valid libpcap packet entry, or `nullptr` on error or EOF
    const FrameBuffer& next();

    uint64_t timestamp() const;

private:
    pcap_t* pcap_handle_;
    pcap_pkthdr frame_header_;

    FrameBuffer cache;

};

