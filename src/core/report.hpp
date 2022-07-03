#pragma once

#include <cmath>


class Report {
public:
    Report() = default;
    
    ~Report() = default;

    Report& operator=( const Report& other );

    void reset();

// metadata
public:
    std::string name;
    uint64_t id = 0; // 0 => error value
    uint64_t timestamp = 0; // time in usec
    enum SOURCE_SENSOR {
        AIS, FUSION, INFRARED, MANUAL, RADAR, RADIO, UNKNOWN, VISUAL, 
    };
    SOURCE_SENSOR source = UNKNOWN;

    // see: https://github.com/schwehr/libais/blob/master/ais/lut.py#L5
    // 15 === 'undefined' navigation status, according to AIS spec
    int status = 15;

// position / orientation
public:
    double latitude = NAN;
    double longitude = NAN;
    double easting = NAN;  // meters to the right of the origin 
    double northing = NAN;  // meters upwards from the origin


// velocity
public:
    /// \brief degrees CW from true north
    double heading = NAN;
    /// \brief degrees CW from true north
    double course = NAN;
    /// \brief meters-per-second along course
    double speed = NAN;

};
