#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#include <proj.h>

#include "report.hpp"

using std::isnan;
using std::atof;

Report::Report( std::string _name, uint64_t _id, double _ts,
                double _x, double _y, double _heading, 
                double _course, double _speed)
    : name(_name), id(_id), timestamp(_ts)
    //, latitude(NAN), longitude(NAN)
    , easting(_x), northing(_y)
    , heading(_heading), course(_course), speed(_speed)
{}

Report& Report::operator=( const Report& other ){
    // these two fields are the minimum-valid-update:
    if( (0 == other.id) || (0 == other.timestamp)){
        return *this;
    }

    id = other.id;
    timestamp = other.timestamp;

    // a report assignment should just be a naive copy-everything
    // only if we condense into a track should we filter data....

    if( 15 != other.status ){
        status = other.status;
    }

    if( ! other.name.empty() ){
        name = other.name;
    }
    if( ! std::isnan(other.latitude) ){
        latitude = other.latitude;
        longitude = other.longitude;
    }
    if( ! std::isnan(other.easting) ){
        easting = other.easting;
        northing = other.northing;
    }
    
    if( ! std::isnan(other.heading) ){
        heading = other.heading;
    }
    if( ! std::isnan(other.course) ){
        course = other.course;
    }
    if( ! std::isnan(speed) ){
        speed = other.speed;
    }

    return *this;
}
