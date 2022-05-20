#pragma once

#include <memory>
#include <string>
#include <cstdint>

using std::unique_ptr;
using std::string;
using std::uint32_t;

class Report;

class Track{
public:
    Track() = delete;
    Track(uint64_t uuid);
    Track(const Track& other);
    ~Track() = default;

    std::string str() const;

    void update( const Report& _report );
    
    //const std::string name;
    const uint64_t id;
    
    Report last_report;
    
};
