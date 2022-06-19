#pragma once

#include <cstdint>
#include <memory>

#include "core/report.hpp"

namespace parsers {
namespace moos {

/// \brief parse each MOOS message from the given text

class MessageParser {

public:
    MessageParser() = default;

    /// \brief parses a text MOOS NODE_REPORT message into a local track-position-report
    /// 
    /// Handles Fields:
    ///     NAME, TIME, X, Y, LAT, LON, SPD, HDG
    /// Ignores Fields:
    ///     INDEX, DEP, LENGTH, MODE, TYPE, YAW
    Report* parse( const std::string& line );

private:
    Report export_;

}; // class parsers::moos::Parser


}  // namespace MOOS
}  // namespace parsers
