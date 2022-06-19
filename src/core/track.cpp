#include <cstdlib>
#include <iostream>
#include <sstream>

#include "report.hpp"
#include "track.hpp"

using namespace std;

Track::Track(uint64_t _id)
    : id(_id)
{}

Track::Track(const Track& other)
    //: name(other.name)
    : id(other.id)
{}

void Track::update( const Report& _report ){
    last_report = _report;

    if( ! _report.name.empty() ){
        name = _report.name;
    }
}

std::string Track::str() const { 
   std::ostringstream buf;
    buf << "[" << id << "][" << name << "]  =>  ";
    buf << "@ {" << last_report.latitude << " N Lat, " << last_report.longitude << " E Lon }";
    buf << "// {" << last_report.easting << " Eas, " << last_report.northing << " Nor }";
    return buf.str();
}
