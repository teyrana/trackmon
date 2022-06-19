#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

#include <spdlog/spdlog.h>

#include "message-parser.hpp"

namespace parsers {
namespace moos{

// ======================= Utility Methods ===================================

typedef std::pair< std::string_view, std::string_view> KeyValuePair;


// trim spaces from the string_view
static inline std::string_view trim(std::string_view v) {
    v.remove_prefix(std::min(v.find_first_not_of(" "), v.size()));
    v.remove_suffix(v.size() - 1 - v.find_last_not_of(" "));
    return v;
}

// get next key-value-pair in the csv message:
KeyValuePair get_next_pair( const std::string& text, size_t& start_index ){

    std::string_view start_view = text;
    start_view.remove_prefix( start_index );

    size_t equals_index = start_view.find('=');
    size_t comma_index = start_view.find(',');

    if( std::string::npos != equals_index ){
        if( std::string::npos == comma_index ){
            comma_index = text.size();
        }

        start_index += comma_index + 1;
        const std::string_view key_view = trim(start_view.substr(0,equals_index));
        const std::string_view value_view = trim(start_view.substr(equals_index+1, (comma_index - 1 - equals_index)));
        return {key_view, value_view};
    }

    start_index = std::string::npos;
    return {"",""};
}


// ======================= Class Methods ===================================
//
// Examples:
// .1. "NAME=gilda,X=16.64,Y=-56.82,SPD=1.24,HDG=311.01,TYPE=mokai,
//     GROUP=alpha,MODE=MODE@ACTIVE:LOITERING,ALLSTOP=clear,INDEX=1066,TIME=1655563263.25,LENGTH=4
//
// .2. "NAME=alpha,TYPE=UUV,TIME=1252348077.59,X=51.71,Y=-35.50,
//     LAT=43.824981,LON=-70.329755,SPD=2.00,HDG=118.85,YAW=118.84754,
//     DEP=4.63,LENGTH=3.8,MODE=MODE@ACTIVE:LOITERING"
//
Report* MessageParser::parse( const std::string& text ){
    // spdlog::debug( "    ==== Generating Target Report ====");
    // spdlog::debug( "    ::|{}|::{}", text.length(), text );

    export_.reset();

    size_t parse_at_index = 0;

    while( parse_at_index < text.length() ) {
        const auto key_value_pair = get_next_pair( text, parse_at_index );

        const std::string_view key = key_value_pair.first;
        const std::string_view value = key_value_pair.second;

        if( key.empty()){
            break;
        }else if("HDG"==key){
            export_.heading = std::atof(value.data());
        }else if("LAT"==key){
            export_.latitude = std::atof(value.data());
        }else if("LON"==key){
            export_.longitude = std::atof(value.data());
        }else if("NAME"==key){
            export_.name = value;
        }else if("SPD"==key){
            export_.speed = std::atof(value.data());
        }else if("TIME"==key){
            double int_part;
            double frac_part = std::modf(std::atof(value.data()), &int_part);
            export_.timestamp = static_cast<uint64_t>(int_part)*1'000'000 + static_cast<uint64_t>(frac_part*1'000'000);
        }else if("X"==key){
            export_.easting = std::atof(value.data());
        }else if("Y"==key){
            export_.northing = std::atof(value.data());
        }else{
            continue;
        //}else if("DEP"==key){
        //}else if("INDEX"==key){
        //}else if("LENGTH"==key){
        //}else if("MODE"==key){
        //}else if("TYPE"==key){
        //}else if("YAW"==key){
        }
    }

    if( std::isnan(export_.course) && !std::isnan(export_.heading)){
        export_.course = export_.heading;
    }else if( std::isnan(export_.heading) && !std::isnan(export_.course)){
        export_.heading = export_.course;
    }

    if(0 == export_.id){
        // generate an guid from a text name
        // for more information, see: https://en.cppreference.com/w/cpp/utility/hash
        export_.id = std::hash<std::string>{}(export_.name);
    }

    return &export_;
}


}  // namespace MOOS
}  // namespace parsers
