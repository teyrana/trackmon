#include <cstdlib>
#include <iostream>

#include "report.hpp"
#include "track.hpp"

using namespace std;

Track::Track(const std::string& _name, uint64_t _id)
    : name(_name)
    , id(_id)
{}

Track::Track(const Track& other)
    : name(other.name)
    , id(other.id)
{}

void Track::update(std::unique_ptr<Report> _report){
    last_report = std::move(_report);
    // _report is left in an undefined state
}