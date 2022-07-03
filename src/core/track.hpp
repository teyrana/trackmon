#pragma once

#include <memory>
#include <string>
#include <cstdint>

using std::unique_ptr;
using std::string;
using std::uint32_t;

class Report;

class Track{
public:
    Track() = delete;
    Track(uint64_t uuid);

    ~Track() = default;

    std::string str() const;

    void update( Report* _report );

public:    
// metadata
    const uint64_t id = 0;
    std::string name = "";
    uint64_t timestamp = 0; // time in usec

    int source = Report::SOURCE_SENSOR::UNKNOWN;
    int status = 15;

// position / orientation
    double latitude = NAN;
    double longitude = NAN;
    /// \brief meters to the right of the origin 
    double easting = NAN;
    /// \brief meters upwards from the origin
    double northing = NAN;
    /// \brief degrees CW from true north
    double heading = NAN;
    /// \brief degrees CW from true north
    double course = NAN;

// velocity
    /// \brief meters-per-second along course
    double speed = NAN;

};
