#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#include "message-parser.hpp"

namespace parsers {
namespace moos{

// ======================= Utility Methods ===================================



// trim spaces from the string_view
static inline std::string_view trim(std::string_view v) {
    v.remove_prefix(std::min(v.find_first_not_of(" "), v.size()));
    v.remove_suffix(v.size() - 1 - v.find_last_not_of(" "));
    return v;
}

// get next key-value-pair in the csv message:
size_t get_next_pair( const std::string_view& source, std::string_view& key, std::string_view& value ){

    size_t equals_index = source.find('=');
    size_t comma_index = source.find(',');

    if( std::string::npos != equals_index ){
        if( std::string::npos == comma_index ){
            comma_index = source.size();
        }

        key = trim(source.substr(0,equals_index));
        value = trim(source.substr(equals_index+1, (comma_index - 1 - equals_index)));
        return (comma_index + 1);
    }

    key = "";
    value = "";
    return std::string::npos;
}


// ======================= Class Methods ===================================

Report* MessageParser::parse( const core::StringBuffer& source ){
    // Examples:
    // .1. "NAME=gilda,X=16.64,Y=-56.82,SPD=1.24,HDG=311.01,TYPE=mokai,
    //     GROUP=alpha,MODE=MODE@ACTIVE:LOITERING,ALLSTOP=clear,INDEX=1066,TIME=1655563263.25,LENGTH=4
    //
    // .2. "NAME=alpha,TYPE=UUV,TIME=1252348077.59,X=51.71,Y=-35.50,
    //     LAT=43.824981,LON=-70.329755,SPD=2.00,HDG=118.85,YAW=118.84754,
    //     DEP=4.63,LENGTH=3.8,MODE=MODE@ACTIVE:LOITERING"

    // spdlog::debug( "    ==== Generating Target Report ====");
    // spdlog::debug( "    ::|{}|::{}", text.length(), text );

    export_.reset();

    export_.timestamp = source.timestamp;
    std::string_view view = source.str;

    size_t parse_at_index = 0;
    std::string_view key;
    std::string_view value;

    while( parse_at_index < source.str.length() ) {
        //start_view.remove_prefix( start_index );
        const auto pair_length = get_next_pair( view, key, value );

        // spdlog::trace("             ::(@{}):[{}]:{}", parse_at_index, key, value );

        view.remove_prefix(pair_length);
        parse_at_index += pair_length;

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
