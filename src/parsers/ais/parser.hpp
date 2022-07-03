#pragma once

#include <array>
#include <cstdint>
#include <tuple>

#include "core/buffers.hpp"
#include "core/report.hpp"

namespace parsers {
namespace ais {

/// \brief parse each AIS message from the linked connector

class Parser {
public:
    Parser() = default;

    /// \brief parses the next NMEA sentence from its source connection
    /// \return true on success; false on failure.
    /// 
    /// On Success: a track report will be sent to the trackcache.
    /// On Failure: Additional information _may_ be printed to stderr
    ///
    /// Further Reference:
    ///   - https://github.com/schwehr/libais
    ///   - https://gpsd.gitlab.io/gpsd/AIVDM.html
    ///   - https://en.wikipedia.org/wiki/Automatic_identification_system#Message_format
    ///   - https://github.com/schwehr/libais/blob/master/src/libais/ais.h
    ///   - https://github.com/schwehr/libais/tree/master/ais
    Report* parse( const core::StringBuffer& line );

private:
    Report export_;

};

}  // namespace ais
}  // namespace parsers