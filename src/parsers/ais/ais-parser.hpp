#pragma once

#include <array>
#include <cstdint>
#include <tuple>

#include "core/report.hpp"

/// \brief parse each AIS message from the linked connector

class AISParser {
public:
    AISParser() = default;

    bool load( uint64_t timestamp, uint8_t* buffer, size_t length );

    Report* parse();

private:

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
    Report* parse_nmea_sentence( const std::string& sentence );

private:
    uint64_t timestamp_ = 0;
    uint8_t* data_start_ = nullptr;
    uint8_t* data_cursor_ = nullptr;
    uint8_t* data_end_ = nullptr;

    Report export_;

};

