#pragma once

#include <array>
#include <string>

#include "core/buffers.hpp"

namespace parsers {
namespace nmea0183 {

/// \brief parser that takes the payload of a network packet and extracts NMEA-0183 sentences
///
class PacketParser {
public:

    PacketParser() = default;
    
    bool empty() const;

    bool load( const core::ForwardBuffer* source );

    /// \brief returns the next network frame
    /// \return on success -- get a byte-pointer to a valid NMEA Line
    ///         on failure -- empty string.  Signifies that no valid lines are available
    core::StringBuffer* next();

public:
    size_t length = 0;

private:
    const uint8_t* find_byte( const uint8_t* start, uint8_t value );

private:
    uint8_t* buffer = nullptr;
    uint8_t* cursor = nullptr;

    core::StringBuffer cache;

};

}  // namespace nmea0183
}  // namespace parsers
