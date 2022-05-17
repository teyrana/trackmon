#pragma once

#include <memory>
#include <string>
#include <cstdint>

using std::unique_ptr;
using std::string;
using std::uint32_t;

class Report;

class Track
{
    public:
        Track() = delete;
        Track(const std::string& _name, uint64_t uuid);
        Track(const Track& other);
        ~Track() = default;

        void update(std::unique_ptr<Report> _report);

        const std::string name;
        const uint64_t id;

        unique_ptr<Report> last_report;

};
