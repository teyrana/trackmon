#include <chrono>

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
    , command(' ')
    , paused(false)
{
    // SIGWINCH ~= SIGnal-WINdow-CHange
    signal(SIGWINCH, resizeHandler);

    // columns.emplace(? "Source", 
    columns.emplace_back("ID", "Id", "%lX", 20);
    //columns.emplace_back("TIME", "Time", "%g", 12);
    columns.emplace_back("AGE", "Time", "%+9.8g", 12);
    columns.emplace_back("NAME", "Name", "%-20s", 20);
    columns.emplace_back("X", "X", "%+9.2g", 10);
    columns.emplace_back("Y", "Y", "%+9.2g", 10);
    
    // mvprintw(0,0,"Source             Time                Name        X / Y            Latitude / Longitude    ");
}

void CursesRenderer::configure(){
}


bool CursesRenderer::is_paused() const {
    return paused;
}

void CursesRenderer::pause() {
    paused = true;
}

void CursesRenderer::resume() {
    paused = false;
}



void CursesRenderer::render_command(){
    // if(hotKeyMode){
    //     mvprintw(LINES-1, 0, ">> ");
    //     mvprintw(LINES-1, 3, command );
    //     printw("\n");
    // }else{
    const int status_render_line = LINES + status_line_offset;
    mvprintw(status_render_line, 0, "command:%c: <result>", command );
    // }
    return;
}

void CursesRenderer::render_options(){
    {  // upper option line:
        mvprintw(LINES + option_upper_line_offset, 0, "==== ");

    }{ // lower option line:
        mvprintw(LINES + option_lower_line_offset, 0, "==== ");
    }
}

void CursesRenderer::render_status_bar(){
    move(LINES + footer_line_offset,0);

    attron(A_REVERSE);
    {
        // chunks of 12
        printw("============ ============ ");
        printw("============ ============ ");
        printw("==== %4d/%4d Tracks ==== ", (int)1, (int)1);
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
    std::time_t current_time = std::time(nullptr);

    if(0 == cache.size()){
        // dummy / placeholder
        mvprintw( header_line_offset, 0, " < No Tracks Received > ");
    } else {
        size_t row = header_line_offset;
        for (auto iter = cache.cbegin(); iter != cache.cend(); ++iter) {
            const uint64_t id = iter->first;
            const Track& track = iter->second;
            
            const Report * const report = track.last_report.get();

            int col = 0;
            for( DisplayColumn& disp : columns ){
                if("AGE" == disp.key){
                    const double age = current_time - report->timestamp;
                    mvprintw( row, col, disp.format.c_str(), age);
                }else if("TIME" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), report->timestamp);
                }else if("ID" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), id);
                }else if("NAME" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), track.name.c_str());
                }else if("X" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), report->x);
                }else if("Y" == disp.key){
                    mvprintw( row, col, disp.format.c_str(), report->y);
                }
                
                col += disp.width;
            }
            ++row;
        }
    }

    return;
}


void CursesRenderer::set_key_command(const char _command){
    command = _command;
}

void CursesRenderer::update(){
    if(paused)
        return;

    clear();

    render_column_headers();

    // render the columns themselves
    render_column_contents();

    // footer
    render_status_bar();
    render_options();
    render_command();

    refresh();    // Print it on to the real screen
}
