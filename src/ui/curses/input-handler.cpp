// Standard Library Includes

// External Dependency Includes
#include <ncurses.h>

// Project Includes
#include "input-handler.hpp"
#include "renderer.hpp"

namespace ui {
namespace curses {

CursesInputHandler::CursesInputHandler(TrackCache& cache)
    : renderer(cache)
{
    configure();
}

void CursesInputHandler::configure(){
    // initialise Ncurses
    if (initscr() == NULL) {
        fprintf(stderr, "Error initializing NCurses: initscr() failed!!\n");
        exit(EXIT_FAILURE);
    }

    renderer.configure();

    // One-character-a-time:
    // Disables the buffering of typed characters by the TTY driver;
    // get a-character-at-a-time input,
    raw();

    // No echo:
    // Suppresses the automatic echoing of typed characters:
    noecho();

    // set reads to be non-blocking
    timeout(0);
    
    // // in milliseconds
    // timeout(500);

    fprintf(stderr, "    >>> Successfully initialized Curses. >>\n");
}

bool CursesInputHandler::handle_input(){
    char key = getch();
    
    if(ERR == key){
        // technically an error, but also the return if no input is available -- 
        // i.e. this is the very common, default case
        return false;
    }

    if( 'q' == key ){
        // normal exit
        shutdownCurses();
        exit(0);
    }

    if(('0' <= key) && ( key <= '9')){
    // ignore number keys
        return false;
    }

    // lowercase all capitals:
    if(('A' <= key) && ( key <= 'Z')){
        key += static_cast<uint8_t>('a' - 'A');
    }

    if((' ' <= key) && ( key <= '}')){
        renderer.set_key_command(key);
        switch(key){
            case ' ':
                renderer.toggle_pause(); break;
            case 'a':
                break;
            case 'h':
                renderer.toggle_help(); break;
            case 'p':
                renderer.toggle_pause(); break;
            default:
                // no-op // debug
                renderer.set_key_result("noop--unsupported key.", 23);
        }
        renderer.render();
        last_update_ = std::chrono::system_clock::now();

        return true;
    }

    return false;
}

void CursesInputHandler::shutdownCurses(){
    // End curses mode
    endwin();
    
    fprintf(stderr, "Program finished: shutting down NCurses.\n\n");
}

std::chrono::system_clock::time_point CursesInputHandler::update( bool changed ){
    if( handle_input() ){
        // noop
    }else if(changed){
        renderer.render();
        last_update_ = std::chrono::system_clock::now();
    } // else{
    //    // noop
    // }

    return last_update_;
}

}  // namespace curses
}  // namespace ui

