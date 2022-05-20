#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <proj.h>

#include "track-cache.hpp"

//PJ* P_for_GIS;
//PJ_COORD a, b;

TrackCache::TrackCache()
//    : contextp(nullptr)
//    , proj(nullptr)
{
//    contextp = proj_context_create();
//    set_origin(42.357591,-71.082075);  // middle of Charles River Basin
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
    
    // if(std::isnan(latitude) || std::isnan(longitude)){
    //     return;
    // }
    // this->origin_latitude = latitude;
    // this->origin_longitude = longitude;

    // // https://mangomap.com/robertyoung/maps/69585/what-utm-zone-am-i-in-#
    // const int zone = (static_cast<int>((origin_longitude + 180.)/6.) + 1) % 60;
    // // fprintf(stderr, "origin (deg): %g, %g\n", origin_latitude, origin_longitude);
    // // fprintf(stderr, "zone:         %d\n", zone);

    // const std::string definition("+proj=utm +zone=" + std::to_string(zone) + " +ellps=GRS80");
    // if(nullptr == proj){
    //     proj_destroy(proj);
    // }
    // proj = proj_create(contextp, definition.c_str());
    
    // //     # preparing projection data
    // //     self.pj_latlong = pyproj.Proj("+proj=latlong +ellps=WGS84")
    // //     self.pj_utm = pyproj.Proj("+proj=utm +ellps=WGS84 +zone=%d" % self._zone)

    // // x, y = pyproj.transform(self.pj_latlong,  self.pj_utm, origin.longitude, origin.latitude)
    // // const double origin_x = NAN;
    // // const double origin_y = NAN;
    // // fprintf(stderr, "origin: %g, %g\n", origin_x, origin_y);
    
    // // fprintf(stderr, ">>> Debug Projection\n");
    // this->origin_latitude = proj_torad(origin_latitude);
    // this->origin_longitude = proj_torad(origin_longitude);
    // // fprintf(stderr, "    .1. Projecting LL: lat: %g,    lon: %g \n", origin_latitude, origin_longitude);
    // // fprintf(stderr, "    .2. As Radians:    lat: %g,    lon: %g\n", proj_torad(origin_latitude), proj_torad(origin_longitude));
    // PJ_COORD c_in = proj_coord( proj_torad(origin_longitude), proj_torad(origin_latitude), 0, 0);

    // PJ_COORD raw_coord = proj_trans( proj, PJ_FWD, c_in);
    // // fprintf( stderr, "    .3. Easting:   %12.2f,    Northing: %12.2f\n", raw_coord.enu.e, raw_coord.enu.n );

    // // PJ_COORD back_coord = proj_trans (proj, PJ_INV, raw_coord);
    // // fprintf( stderr, "    .4. Back Check:   lat: %g,    lon: %g\n", back_coord.lp.phi, back_coord.lp.lam);

    // origin_easting = raw_coord.enu.e;
    // origin_northing = raw_coord.enu.n;
    // // fprintf(stderr, "    .5. Projecting to UTM: %g, %g \n", origin_easting, origin_northing);
}

size_t TrackCache::size() const {
    return index.size();
}

bool TrackCache::update(const Report& report){
    //auto report = Report::make(text, proj, origin_easting, origin_northing);

    // create new Track, if missing
    const auto [track_entry, _found] = index.try_emplace(report.id, report.id);

    // faster than, but equivalent to: `index[report->id].update(...)`
    track_entry->second.update( report );

    return true;
}

std::string TrackCache::to_string() const { 
    std::ostringstream buf;

    // print with X -> left; Y -> Up
    buf << "======== ======= ======= Cache has " << index.size() << " entries ======= ======= =======\n";

    for( const auto& [_id, each_track] : index ){
        buf << "         [" << each_track.id << "] => " << each_track.str() << '\n';
    }
    buf << std::endl;

    return buf.str();
}


TrackCache::~TrackCache(){

    // libproj cleanup
//    proj_destroy(proj);
//    proj_context_destroy(contextp);

    // Probably not necessary.
    // // for track in index:
    // for (auto it = index.begin(); it != index.end(); ++it) {
    //     uint64_t key = it->first;
    //     Track* track = &it->second;
    //     //     (1) remove track from index
    //     index.erase(key);
    //     //     (2) delete each track
    //     delete track;
    // }
    return;
}
