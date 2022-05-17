#pragma once

#include <string>

using std::string;

class DisplayColumn 
{

public:
    DisplayColumn() = delete;
    DisplayColumn(  const std::string _key, 
                    const std::string _title,
                    const std::string _format,
                    size_t _width);
    // ~DisplayColumn() = default;

    const std::string key;
    const std::string title;
    const std::string format;
    size_t width;
    
};
