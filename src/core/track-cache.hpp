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

        void set_origin( double latitude, double longitude );

        size_t size() const;

        std::string to_string() const;

        bool update( const Report& report );

    private:
        std::map<uint64_t, Track> index;

};
