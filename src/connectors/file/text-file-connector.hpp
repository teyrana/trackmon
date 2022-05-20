#pragma once

#include <fstream>
#include <streambuf>
#include <string>

/// \brief simple file connector that reads a line-at-time from each input file
class TextFileConnector {
public:

    TextFileConnector( const std::string& filename );

    bool good() const;

    bool open( const std::string& filename );

    /// \brief returns the next line of the file
    /// \return get a pointer to a const std::string 
    const std::string* next();

private:
    // per-class cache -- !NOT THREAD SAFE!
    std::string _each_line;

    std::ifstream _source;
};
