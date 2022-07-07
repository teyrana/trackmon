#include <string>

using std::string;

#include "display-column.hpp"

namespace ui {

const DisplayColumn DisplayColumn::ID(       "ID",    "Id",        " %10ld ",   10);
const DisplayColumn DisplayColumn::TIME(     "TIME",  "Time",      " %20lu ",   20);
const DisplayColumn DisplayColumn::AGE(      "AGE",   "Time",      " %20lu ",   20);
const DisplayColumn DisplayColumn::NAME(     "NAME",  "Name",      " %32s ",    32);
const DisplayColumn DisplayColumn::EASTING(  "EAST",  "Easting",   " %+12.3f ", 12);
const DisplayColumn DisplayColumn::NORTHING( "NORTH", "Northing",  " %+12.3f ", 12);
const DisplayColumn DisplayColumn::LATITUDE( "LAT",   "Latitude",  " %+12.3f ", 12);
const DisplayColumn DisplayColumn::LONGITUDE("LON",   "Longitude", " %+12.3f ", 12);

DisplayColumn::DisplayColumn(const std::string _key, 
                    const std::string _title,
                    const std::string _format,
                    size_t _width)
    : key(_key)
    , title(_title)
    , format(_format)
    , width(_width)
{}

// DisplayColumn::~DisplayColumn(){}

const DisplayColumn* DisplayColumn::get( const std::string& key ) {
    if( key == ID.key ){
        return &ID;
    }else if( key == TIME.key ){
        return &TIME;
    }else if( key == AGE.key ){
        return &AGE;
    }else if( key == NAME.key ){
        return &NAME;
    }else if( key == EASTING.key ){
        return &EASTING;
    }else if( key == NORTHING.key ){
        return &NORTHING;
    }else if( key == LATITUDE.key ){
        return &LATITUDE;
    }else if( key == LONGITUDE.key ){
        return &LONGITUDE;
    }

    return nullptr;
}

}  // namespace ui
