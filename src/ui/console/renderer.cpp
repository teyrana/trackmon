#include <cassert>
#include <chrono>
#include <cstring>

#include <fmt/core.h>

#include "renderer.hpp"


namespace ui {
namespace console {

ConsoleRenderer::ConsoleRenderer(TrackCache& _cache)
    : cache(_cache)
    , render_live(true)
{


    columns.push_back(DisplayColumn::ID);
    columns.push_back(DisplayColumn::TIME);
    columns.push_back(DisplayColumn::NAME);
    columns.push_back(DisplayColumn::LATITUDE);
    columns.push_back(DisplayColumn::LONGITUDE);
    columns.push_back(DisplayColumn::EASTING);
    columns.push_back(DisplayColumn::NORTHING);
}

void ConsoleRenderer::configure(){
}

bool ConsoleRenderer::paused() const {
    return not render_live;
}


void ConsoleRenderer::render_column_headers(){
    fmt::print(prefix);
    for( const DisplayColumn& col : columns ){
        fmt::print( "[{0:>{1}s}]", col.title, col.width );
    }
    putc('\n',stdout);
    render_horizontal_rule();
}

void ConsoleRenderer::render_column_contents(){
    auto current_time = std::chrono::system_clock::now();

    if(0 == cache.size()){
        // dummy / placeholder
        fmt::print( " < No Tracks Received > ");
    } else {
        for (auto iter = cache.cbegin(); iter != cache.cend(); ++iter) {
            const Track& track = iter->second;

            fmt::print(prefix);

            std::array<char,512> buffer;
            char* write_cursor = buffer.data();
            const char* buffer_end = buffer.data() + buffer.size();

            for( const DisplayColumn& col : columns ){
                size_t write_count;
                const int remaining = buffer_end - write_cursor;
                const char* format = col.format.c_str();

                if("AGE" == col.key){
                    const std::chrono::time_point age = current_time - std::chrono::microseconds(track.timestamp);
                    const uint64_t age_usec_count = age.time_since_epoch().count();
                    // write_count = snprintf( write_cursor, remaining, format, age_usec_count );
                    const uint32_t sec_only = static_cast<uint32_t>(age_usec_count / 1'000'000u);
                    const uint32_t usec_only = static_cast<uint32_t>(age_usec_count % 1'000'000u);
                    write_count = snprintf( write_cursor, remaining, "  %12u.%6u ", sec_only, usec_only );
                }else if("EAST" == col.key){
                    write_count = snprintf( write_cursor, remaining, format, track.northing );
                }else if("ID" == col.key){
                    write_count = snprintf( write_cursor, remaining, format, track.id );
                }else if("LAT" == col.key){
                    write_count = snprintf( write_cursor, remaining, format, track.latitude );
                }else if("LON" == col.key){
                    write_count = snprintf( write_cursor, remaining, format, track.longitude );
                }else if("NAME" == col.key){
                    write_count = snprintf( write_cursor, remaining, format, track.name.c_str() );
                }else if("NORTH" == col.key){
                    write_count = snprintf( write_cursor, remaining, format, track.easting );
                }else if("TIME" == col.key){
                    // write_count = snprintf( write_cursor, remaining, format, track.timestamp );
                    const uint32_t sec_only = static_cast<uint32_t>( track.timestamp / 1'000'000u);
                    const uint32_t usec_only = static_cast<uint32_t>( track.timestamp % 1'000'000u);
                    write_count = snprintf( write_cursor, remaining, "  %12u.%6u ", sec_only, usec_only );
                }
                write_cursor += write_count;
            }

            fmt::print("{}\n", buffer.data() );
        }
    }

    return;
}

void ConsoleRenderer::render_horizontal_rule() {
    fmt::print( "{0}{1:=>{2}}\n", prefix, '=', max_render_width );
}

bool ConsoleRenderer::toggle_pause(){
    if( render_live ){
        fmt::print( "Pause Track Updates." );
        render_live = false;
    }else{
        fmt::print( "Resume Track Updates." );
        render_live = true;
    }
    return render_live;
}


void ConsoleRenderer::render(){

    // header
    render_column_headers();

    // render the columns themselves
    // if( render_live ){
    render_column_contents();
    // }

}

}  // namespace console
}  // namespace ui
