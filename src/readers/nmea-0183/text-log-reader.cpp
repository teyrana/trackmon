#include <filesystem>
#include <iostream>

#include "text-log-reader.hpp"

namespace readers {
namespace NMEA0183 {

TextLogReader::TextLogReader( const std::string& filename ){
    open(filename);
}

bool TextLogReader::good() const {
    return _source.good();
}

bool TextLogReader::open( const std::string& filename ){
    if( ! std::filesystem::exists(std::filesystem::path(filename))){
        std::cerr << "?!? file is missing: " << filename << '\n';
        std::cerr << "::cwd: " << std::filesystem::current_path().string() << '\n';
        return false;
    }

    _source.open(filename);

    return _source.good();
}

const std::string* TextLogReader::next() {
    _source >> _each_line;

    if( _source.good() ){   
        return &_each_line;
    }

    return nullptr;
}

}  // namespace readers
}  // namespace NMEA0183
