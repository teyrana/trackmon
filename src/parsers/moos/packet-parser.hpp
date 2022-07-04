#pragma once

#include "core/buffers.hpp"

namespace parsers {
namespace moos {

/// \brief parse each AIS message from the linked connector

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
    std::string extract_string();

private:
    uint8_t* buffer = nullptr;
    uint8_t* cursor = nullptr;

    core::StringBuffer cache;

}; // class parsers::moos::PacketParser

}  // namespace moos
}  // namespace parsers
