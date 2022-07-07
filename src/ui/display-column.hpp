#pragma once

#include <map>
#include <string>

namespace ui {

class DisplayColumn {
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


public:
    static const DisplayColumn* get( const std::string& k );

public:
    const static DisplayColumn ID;
    const static DisplayColumn TIME;
    const static DisplayColumn AGE;
    const static DisplayColumn NAME;
    const static DisplayColumn EASTING;
    const static DisplayColumn NORTHING;
    const static DisplayColumn LATITUDE;
    const static DisplayColumn LONGITUDE;

    // columns.emplace(? "Source", 

    // static std::map<std::string,const DisplayColumn&> lookup;
};


}  // namespace ui