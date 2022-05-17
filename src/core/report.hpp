#pragma once

#include <proj.h>

#include <memory>

class Report {
public:
    static std::unique_ptr<Report> make(const std::string_view text, PJ* proj, 
                                        double origin_offset_easting, double origin_offset_northing);
    
    Report() = delete;
    Report(std::string _name, uint64_t _id, double _ts,
                double _x, double _y, double _heading,
                double _course, double _speed);
    ~Report() = default;

// metadata
    const std::string name;
    const uint64_t id;
    const double timestamp;

// position / orientation
    const double x;  // meters to the right of the origin 
    const double y;  // meters upwards from the origin
    /// /brief degrees CW from true north
    const double heading;

// velocity
    /// /brief degrees CW from true north
    const double course;
    const double speed;

private:
    static const std::hash<std::string> hasher;

};
