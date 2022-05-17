#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#include <proj.h>

#include "report.hpp"

using std::isnan;
using std::atof;

#ifdef DEBUG
extern FILE* logfile;
#endif

Report::Report(std::string _name, uint64_t _id, double _ts,
                double _x, double _y, double _heading,
                double _course, double _speed)
    : name(_name), id(_id), timestamp(_ts)
    , x(_x), y(_y), heading(_heading)
    , course(_course), speed(_speed)
{
    // no-op
}

const std::hash<std::string> Report::hasher;


// trim spaces from the string_view
static inline std::string_view trim(std::string_view v) {
    v.remove_prefix(std::min(v.find_first_not_of(" "), v.size()));
    v.remove_suffix(v.size() - 1 - v.find_last_not_of(" "));
    return v;
}

//  " NAME=alpha,TYPE=UUV,TIME=1252348077.59,X=51.71,Y=-35.50,
//    LAT=43.824981,LON=-70.329755,SPD=2.00,HDG=118.85,YAW=118.84754,
//    DEP=4.63,LENGTH=3.8,MODE=MODE@ACTIVE:LOITERING"
std::unique_ptr<Report> Report::make(const std::string_view text, 
                                        PJ* proj, double origin_easting, double origin_northing)
{
    // fprintf(logfile, "==== Generating Target Report ====\n");
    // fprintf(logfile, "    source: %s\n", text.data());
        
    std::string name("");
    size_t id = 0;

    double timestamp = NAN;

    // position
    double x = NAN;
    double y = NAN;
    double latitude = NAN;
    double longitude = NAN;

    // orientation
    /// /brief degrees CW from true north
    double heading = NAN;

    // velocity
    /// /brief degrees CW from true north
    double course = NAN;
    double speed = NAN;

    const auto npos = std::string::npos;

    std::string::size_type start_pos = 0;
    std::string::size_type end_pos = text.find(',');
    
    while( npos != end_pos) {
        std::string_view pair = text.substr(start_pos, end_pos-start_pos);

        start_pos = end_pos + 1;
        end_pos = text.find(',', start_pos);
        if(npos == end_pos){
            if(start_pos < text.size()){
                end_pos = text.size();
            }
        }

        // process filed
        auto split_index = pair.find('=');
        if(npos == split_index){
            continue;
        }

        auto key = trim(pair.substr(0,split_index));
        auto value = trim(pair.substr(split_index+1));
        
        // DEBUG
        // std::cerr << "      [" << key << "]= '" << value << "'" << std::endl;

        if("NAME"==key){
            name = value;
        // }else if("TYPE"==key){
        }else if("TIME"==key){
            timestamp = std::atof(value.data());
        }else if("X"==key){
            x = std::atof(value.data());
        }else if("Y"==key){
            y = std::atof(value.data());
        }else if("LAT"==key){
            latitude = std::atof(value.data());
        }else if("LON"==key){
            longitude = std::atof(value.data());
        }else if("SPD"==key){
            speed = std::atof(value.data());
        }else if("HDG"==key){
            heading = std::atof(value.data());
        //}else if("YAW=118.84754,
        //}else if("DEP=4.63,
        //}else if("LENGTH=3.8,
        // }else if("MODE=MODE@ACTIVE:LOITERING"
        }
    }
    
    if( isnan(course) && !isnan(heading)){
        course = heading;
    }else if( isnan(heading) && !isnan(course)){
        heading = course;
    }

    if( isnan(x) || isnan(y)){
        // fprintf(logfile, "<< Missing: x,y\n");
        // fprintf(logfile, "    source: %s\n", text.data());
        // fprintf(logfile, "    Projecting LL: %g, %g \n", latitude, longitude);
        PJ_COORD c_in = proj_coord( proj_torad(longitude), proj_torad(latitude), 0, 0);

        // fprintf(logfile, "    (Radians) LL: %g, %g \n", proj_torad(longitude), proj_torad(latitude));

        PJ_COORD raw_coord = proj_trans( proj, PJ_FWD, c_in);
        
        // fprintf( logfile, "    Easting: %.3f, Northing: %.3f\n", raw_coord.enu.e, raw_coord.enu.n );

        //raw_x, raw_y = pyproj.transform(self.pj_latlong, self.pj_utm, source.longitude, source.latitude)
        
        // PJ_COORD back_coord = proj_trans (proj, PJ_INV, raw_coord);
        // fprintf(logfile, "    Back-Check: Longitude: %g, Latitude: %g\n", back_coord.lp.lam, back_coord.lp.phi);

        x = raw_coord.enu.e + origin_easting;
        y = raw_coord.enu.n + origin_northing;
        // fprintf(logfile, "    Projecting to UTM: %g, %g \n", x, y);
        // fflush(logfile);
    }
    
    if(0 == id){
        id = hasher(name);
    }

    return std::make_unique<Report>(name, id, 
            timestamp,
            x, y, heading,
            course, speed);
}
