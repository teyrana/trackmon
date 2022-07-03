#include <cassert>
#include <chrono>
#include <cstring>

#include <ncurses.h>
#include <signal.h>

#include "curses-renderer.hpp"

/* If an xterm is resized the contents on your text windows might be messed up.
To handle this gracefully you should redraw all the stuff based on the new
height and width of the screen. When resizing happens, your program is sent
a SIGWINCH signal. You should catch this signal and do redrawing accordingly.
*/
void resizeHandler(int sig)
{
    int h, w;

    // this simply doesn't update h&w under OSX when using terminal
    getmaxyx(stdscr, h, w);
    fprintf(stderr, "Resizing: (h= %d, w= %d )\n", h, w);
    fprintf(stderr, "Resizing: (LINES= %d, COLS= %d )\n", LINES, COLS);
    refresh();
}

CursesRenderer::CursesRenderer(TrackCache& _cache)
    : cache(_cache)
    , command_key(' ')
    , command_result("\0")
    , render_live(true)
    , render_help(true)
{
    // SIGWINCH ~= SIGnal-WINdow-CHange
    signal(SIGWINCH, resizeHandler);

    // columns.emplace(? "Source", 
    columns.emplace_back("ID", "Id", "%ld", 20);
    // columns.emplace_back("TIME", "Time", "%g", 12);
    columns.emplace_back("AGE", "Time", "%+9.8g", 12);
    columns.emplace_back("NAME", "Name", "%-20s", 20);
    //columns.emplace_back("X", "X", "%+9.2g", 10);
    //columns.emplace_back("Y", "Y", "%+9.2g", 10);

    columns.emplace_back("LAT", "Latitude", "%+9.2g", 10);
    columns.emplace_back("LON", "Longitude", "%+9.2g", 10);
    
    // mvprintw(0,0,"Source             Time                Name        X / Y            Latitude / Longitude    ");
}

void CursesRenderer::configure(){
}

bool CursesRenderer::paused() const {
    return not render_live;
}

void CursesRenderer::render_command(){
    // if(hotKeyMode){
    //     mvprintw(LINES-1, 0, ">> ");
    //     mvprintw(LINES-1, 3, command );
    //     printw("\n");
    // }else{
    const int status_render_line = LINES + status_line_offset;
    mvprintw(status_render_line, 0, "command:(%c)::  %s", command_key, command_result );
    // }
    return;
}

void CursesRenderer::render_options(){
    if( render_help ){
        // upper option line:
        mvprintw(LINES + option_upper_line_offset, 0, "(p)ause");

        // lower option line:
        mvprintw(LINES + option_lower_line_offset, 0, "(h)elp    (q)uit ");
    }
}

void CursesRenderer::render_status_bar(){
    if( render_help ){
        move(LINES + footer_expand_line_offset, 0);
    }else{
        move(LINES + footer_minimal_line_offset, 0);
    }

    attron(A_REVERSE);
    {
        // chunks of 12 '=' characters
        if( render_live ){
            printw("[ >> ]");
        }else{
            printw("[ || ]");
        }
        printw("============ ============ ");
        printw("============ ============ ");
        printw("==== %4d/%4d Tracks ==== ", (int)0, (int)cache.size());
    }
    attroff(A_REVERSE);
    return;
}

// void CursesRenderer::rightFillStatusBar(){
//     int x = getcurx(stdscr);
//     // int y = getcury(stdscr);

//     while(12 < (COLS - x)){
//         printw("============");
//         x += 12;
//     }

//     // logically it should be 0, but that would crash ncurses :(
//     while( 1 < (COLS - x)){
//         printw("=");
//         ++x;
//     }
// }

void CursesRenderer::render_column_headers(){
    // mvprintw(0,0,"Source             Time                Name        X / Y            Latitude / Longitude    ");
    int col = 0;
    for( DisplayColumn& disp : columns ){
        mvprintw( 0, col, disp.title.c_str());
        col += disp.width;
    }

    // draw a horizontal rule <hr> between the Column Titles and the data
    move( 1, 0);
    hline(ACS_HLINE, 999);
}

void CursesRenderer::render_column_contents(){
    auto current_time = std::chrono::system_clock::now();

    if(0 == cache.size()){
        // dummy / placeholder
        mvprintw( header_line_offset, 0, " < No Tracks Received > ");
    } else {
        size_t row = header_line_offset;
        for (auto iter = cache.cbegin(); iter != cache.cend(); ++iter) {
            const uint64_t id = iter->first;
            const Track& track = iter->second;

            int col = 0;
            for( DisplayColumn& disp : columns ){
                if("AGE" == disp.key){
                    const auto age = current_time - std::chrono::microseconds(track.timestamp);
                    mvprintw( row, col, disp.format.c_str(), age);
                }else if("TIME" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), track.timestamp);
                }else if("ID" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), id);
                // }else if("NAME" == disp.key){
                // mvprintw( row, col, disp.format.c_str(), track.name.c_str());
                }else if("LAT" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), track.latitude);
                }else if("LON" == disp.key){
                mvprintw( row, col, disp.format.c_str(), track.longitude);
                }else if("X" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), track.easting);
                }else if("Y" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), track.northing);
                }
                
                col += disp.width;
            }
            ++row;
        }
    }

    return;
}

bool CursesRenderer::toggle_pause(){
    if( render_live ){
        std::memcpy( command_result, "Pause Track Updates.", 21 );
        render_live = false;
    }else{
        std::memcpy( command_result, "Resume Track Updates.", 22 );
        render_live = true;
    }
    return render_live;
}

bool CursesRenderer::toggle_help(){
    if( render_help ){
        std::memcpy( command_result, "Hiding command help.", 21 );
        render_help = false;
    }else{
        std::memcpy( command_result, "Showing command help.", 22 );
        render_help = true;
    }
    return render_help;
}

void CursesRenderer::set_key_command(const char _command) {
    command_key = _command;
}

void CursesRenderer::set_key_result(const char* _result, size_t length ){
    assert( length < CursesRenderer::command_result_buffer_length );
    memcpy( this->command_result, _result, length );
}

void CursesRenderer::render(){
    clear();

    // header
    render_column_headers();

    // render the columns themselves
    // if( render_live ){
    render_column_contents();
    // }

    // footer
    render_status_bar();
    render_options();
    render_command();

    refresh();    // Print it on to the real screen
}
