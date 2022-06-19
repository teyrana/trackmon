#pragma once

#include <array>
#include <string>

#include "readers/pcap/frame-buffer.hpp"

namespace parsers {
namespace nmea0183 {

/// \brief parser that takes the payload of a network packet and extracts NMEA-0183 sentences
///
class PacketParser {
public:

    PacketParser() = default;
    
    bool empty() const;

    uint32_t length() const;

    bool load( const readers::pcap::FrameBuffer* source );

    /// \brief returns the next network frame
    /// \return on success -- get a byte-pointer to a valid NMEA Line
    ///         on failure -- empty string.  Signifies that no valid lines are available
    std::string next();

    uint64_t timestamp() const;

private:
    const uint8_t* find_byte( const uint8_t* start, uint8_t value );

private:
    const readers::pcap::FrameBuffer* cache = nullptr;
    uint8_t* cursor = nullptr;

};

}  // namespace nmea0183
}  // namespace parsers
