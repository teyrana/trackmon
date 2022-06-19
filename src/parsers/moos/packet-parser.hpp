#pragma once

#include <cstdint>


#include "readers/pcap/frame-buffer.hpp"

namespace parsers {
namespace moos {

/// \brief parse each AIS message from the linked connector

class PacketParser {
public:
    PacketParser() = default;

    bool empty() const;

    bool load( const readers::pcap::FrameBuffer* source );

    /// \brief returns the next network frame
    /// \return on success -- get a byte-pointer to a valid NMEA Line
    ///         on failure -- empty string.  Signifies that no valid lines are available
    std::string next();

public:
    uint64_t timestamp = 0;
    size_t length = 0;

private:
    std::string extract_string();

private:
    uint8_t* buffer = nullptr;
    uint8_t* cursor = nullptr;

}; // class parsers::moos::PacketParser

}  // namespace moos
}  // namespace parsers
