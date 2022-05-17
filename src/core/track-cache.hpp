#pragma once

#include <map>
#include <memory>

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

        void set_origin(double latitude, double longitude);

        size_t size() const;

        bool update(const std::string_view text);

    private:
        double origin_latitude;
        double origin_longitude;
        double origin_easting;
        double origin_northing;

        PJ_CONTEXT *contextp;
        PJ* proj;
        std::map<uint64_t, Track> index;

};
