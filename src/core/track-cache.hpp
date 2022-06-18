#pragma once

#include <map>
#include <memory>

#include <proj.h>

#include "report.hpp"
#include "track.hpp"


typedef std::map<uint64_t, Track>::const_iterator cache_iterator;

class TrackCache
{
public:
    TrackCache();
    
    ~TrackCache();
    
    cache_iterator cbegin() const;
    
    cache_iterator cend() const;

    Track * const get(uint64_t id) const;

    void set_origin( double latitude, double longitude );

    size_t size() const;

    std::string to_string() const;

    bool update( Report& report );


private:
    // Transform UTM Easting/Northing to Latitude/Longitude
    bool project_to_global( double easting_in, double northing_in, double& latitude_out, double& longitude_out);

    // Transform Latitude/Longitude to UTM Easting/Northing
    bool project_to_local( double latitude_in, double longitude_in, double& easting_out, double& northing_out );
    Report& project_to_local( Report& rpt );
    PJ_COORD project_to_local( const PJ_COORD& in_coords );

    // Transform Latitude/Longitude to UTM Easting/Northing
    bool project_to_UTM( double latitude_in, double longitude_in, double& easting_out, double& northing_out );

private:
    /// Coordinates of local-origin
    /// (units: latitude, longitude)
    PJ_COORD anchor;

    /// Offset from UTM-origin to local-origin
    /// (units: easting, northing
    PJ_COORD offset;
    
    PJ_CONTEXT* context;
    PJ* projection;

    bool should_project;
    
    std::map<uint64_t, Track> index;

};
