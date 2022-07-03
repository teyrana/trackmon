#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#include <proj.h>

#include "report.hpp"

using std::isnan;
using std::atof;


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

void Report::reset(){

    id = 0;
    name = "";

    timestamp = 0;
    source = UNKNOWN;
    status = 15;

    latitude = NAN;
    longitude = NAN;
    easting = NAN;
    northing = NAN;
    
    heading = NAN;
    course = NAN;
    speed = NAN;
}
