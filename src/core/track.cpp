#include <cstdlib>
#include <iostream>
#include <sstream>

#include <fmt/format.h>

#include "report.hpp"
#include "track.hpp"

using namespace std;

Track::Track(uint64_t _id)
    : id(_id)
{}

void Track::update(  Report* report ){
    if( (0 == report->id) || (0 == report->timestamp)){
        // these two fields are the minimum-valid-update:
        return;
    }else if( id != report->id ){
        // id mismatch! This is an error
        return;
    }

    timestamp = report->timestamp;

    // a report assignment should just be a naive copy-everything
    // only if we condense into a track should we filter data....

    if( 15 != report->status ){
        status = report->status;
    }

    if( name.empty() && (!report->name.empty()) ){
        name = report->name;
    }
    if( ! std::isnan(report->latitude) ){
        latitude = report->latitude;
        longitude = report->longitude;
    }
    if( ! std::isnan(report->easting) ){
        easting = report->easting;
        northing = report->northing;
    }
    
    if( ! std::isnan(report->heading) ){
        heading = report->heading;
    }
    if( ! std::isnan(report->course) ){
        course = report->course;
    }
    if( ! std::isnan(speed) ){
        speed = report->speed;
    }
}

std::string Track::str() const { 
   std::ostringstream buf;
    buf << "[" << id << "][" << name << "]  =>  ";
    buf << "@ {" << latitude << " N Lat, " << longitude << " E Lon }";
    buf << "// {" << easting << " Eas, " << northing << " Nor }";
    return buf.str();
}
