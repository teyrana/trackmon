#include <filesystem>
#include <iostream>

#include "text-file-connector.hpp"

TextFileConnector::TextFileConnector( const std::string& filename ){
    open(filename);
}

bool TextFileConnector::good() const {
    return _source.good();
}

bool TextFileConnector::open( const std::string& filename ){
    if( ! std::filesystem::exists(std::filesystem::path(filename))){
        std::cerr << "?!? file is missing: " << filename << '\n';
        std::cerr << "::cwd: " << std::filesystem::current_path().string() << '\n';
        return false;
    }

    _source.open(filename);

    return _source.good();
}

const std::string* TextFileConnector::next() {
    _source >> _each_line;

    if( _source.good() ){   
        return &_each_line;
    }

    return nullptr;
}