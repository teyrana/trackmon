#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <proj.h>

#include "track-cache.hpp"


TrackCache::TrackCache()
    : anchor({NAN,NAN,NAN,NAN})
    , offset({NAN,NAN,NAN,NAN})
    , context(nullptr)
    , projection(nullptr)
    , should_project(false)
{
    context = proj_context_create();
}

TrackCache::~TrackCache(){
    // libproj cleanup
    proj_destroy(projection);
    projection = nullptr;
    proj_context_destroy(context);
    context = nullptr;

    assert( nullptr == projection );
    assert( nullptr == context );

    // for (auto it = index.begin(); it != index.end(); ++it) {
    //     uint64_t key = it->first;
    //     Track* track = &it->second;
    //     index.erase(key);
    //     delete track;
    // }

    return;
}

cache_iterator TrackCache::cbegin() const {
    return index.cbegin();
}

cache_iterator TrackCache::cend() const {
    return index.cend();
}

Track* const TrackCache::get(uint64_t id) const {
    return nullptr;
}

void TrackCache::set_origin(double latitude, double longitude) {
    
    if(std::isnan(latitude) || std::isnan(longitude)){
        return;
    }
    anchor = proj_coord( latitude, longitude, 0, 0 );

    // https://mangomap.com/robertyoung/maps/69585/what-utm-zone-am-i-in-#
    const int zone = (static_cast<int>((longitude + 180.)/6.) + 1) % 60;
    // fprintf(stdout, "    ::LL > UTM Origin (deg): %g, %g  ==>  zone: %d\n",
    //                    anchor.latitude, anchor.longitude, zone);

    char projection_definition[36];
    snprintf(projection_definition, 36, "+proj=utm +zone=%d +datum=WGS84", zone );
    //fprintf(stdout, "    ::proj-def-wkt: %s\n", projection_definition ):
    projection = proj_create_crs_to_crs( context, "EPSG:4326", projection_definition, nullptr );

    // Transform Latitude/Longitude to UTM Easting/Northing
    offset = proj_trans( projection, PJ_FWD, anchor );

    // successfully generated projection information:
    should_project = true;
    return;

    // {
    //     // DEBUG
    //     const double latitude = anchor.lp.lam;
    //     const double longitude = anchor.lp.phi;
    //     fprintf(stdout, "    ::Lat/Lon Anchor:(deg): %9.6f, %9.6f\n", latitude, longitude );
    //     fprintf(stdout, "    ::UTM Offset:(m):       %9.2f, %9.2f\n", offset.enu.e, offset.enu.n );
    //     // DEBUG

    //     { // CHECK
    //         const double latitude = anchor.lp.lam;
    //         const double longitude = anchor.lp.phi;
    //         double check_1_easting = NAN;
    //         double check_1_northing = NAN;
    //         project_to_UTM( latitude, longitude, check_1_easting, check_1_northing );
    //         fprintf(stdout, "    :?:UTM:(m):             %9.2f, %9.2f\n", check_1_easting, check_1_northing );
    //     }{ // CHECK
    //         const double latitude = anchor.lp.lam;
    //         const double longitude = anchor.lp.phi;
    //         double check_2_easting = NAN;
    //         double check_2_northing = NAN;
    //         project_to_local( latitude, longitude, check_2_easting, check_2_northing );
    //         fprintf(stdout, "    :?:Local:(m):           %9.2f, %9.2f\n", check_2_easting, check_2_northing );
    //     }{ // CHECK
    //         double check_3_latitude = NAN;
    //         double check_3_longitude = NAN;
    //         project_to_global( offset.enu.e, offset.enu.n, check_3_latitude, check_3_longitude );
    //         fprintf(stdout, "    :?: Lat / Lon:          %9.6f, %9.6f\n", check_3_latitude, check_3_longitude );
    //     } // CHECK
    // }
}

size_t TrackCache::size() const {
    return index.size();
}

bool TrackCache::update( Report& report){

    // create new Track, if missing
    const auto [track_entry, _found] = index.try_emplace(report.id, report.id);

    if( should_project ){
        report = project_to_local( report );
    }

    // faster than, but equivalent to: `index[report->id].update(...)`
    track_entry->second.update( report );

    return true;
}

bool TrackCache::project_to_global( double easting, double northing, double& latitude, double& longitude ){
    PJ_COORD source = proj_coord( easting, northing, 0, 0 );
    PJ_COORD result =  proj_trans( projection, PJ_INV, source );
    latitude = result.lp.lam;
    longitude = result.lp.phi;
    return true;
}

bool TrackCache::project_to_local( double latitude, double longitude, double& easting, double& northing ){
    PJ_COORD source = proj_coord( latitude, longitude, 0, 0 );
    PJ_COORD result = project_to_local( source );
    easting = result.enu.e;
    northing = result.enu.n;
    return true;
}

Report& TrackCache::project_to_local( Report& report ){
    PJ_COORD source = proj_coord( report.latitude, report.longitude, 0, 0 );
    PJ_COORD result = project_to_local( source );
    report.easting = result.enu.e;
    report.northing = result.enu.n;
    return report;
}

PJ_COORD TrackCache::project_to_local( const PJ_COORD& input_coordinates ){
    PJ_COORD output_coordinates = proj_trans( projection, PJ_FWD, input_coordinates );
    output_coordinates.enu.e -= offset.enu.e;
    output_coordinates.enu.n -= offset.enu.n;
    return output_coordinates;
}

bool TrackCache::project_to_UTM( double latitude, double longitude, double& easting, double& northing ){
    PJ_COORD source = proj_coord( latitude, longitude, 0, 0 );
    PJ_COORD result = proj_trans( projection, PJ_FWD, source );
    easting = result.enu.e;
    northing = result.enu.n;
    return true;
}

std::string TrackCache::to_string() const { 
    std::ostringstream buf;

    // print with X -> left; Y -> Up
    buf << "======== ======= ======= Cache has " << index.size() << " entries ======= ======= =======\n";
    for( const auto& [_id, each_track] : index ){
        buf << "         " << each_track.str() << '\n';
    }
    buf << std::endl;

    return buf.str();
}


