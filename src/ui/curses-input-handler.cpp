#include <ncurses.h>

#include "curses-input-handler.hpp"
#include "curses-renderer.hpp"

CursesInputHandler::CursesInputHandler(TrackCache& cache)
    : renderer(cache)
{}

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

    fprintf(stderr, ">> Successfully initialized Curses. >>\n");
}

bool CursesInputHandler::handle_input(){
    const char key = getch();
    if(ERR == key){
        // technically an error, but also the return if no input is available -- 
        // i.e. this is the very common, default case
        return false;
    }
    
    if('q' == key || 17 == key){
        // normal exit
        // 17 == ctrl-q on OSX/Darwin/Terminal
        shutdownCurses();
        exit(0);
    }else if(' ' == key){
        // pause rendering
        if(renderer.is_paused()){
            renderer.resume();
        }else{
            renderer.pause();
        }
    }else{
        return handle_option_key(key);
    }
    return false;
}

bool CursesInputHandler::handle_option_key(char key){
    if(('0' <= key) && ( key <= '9')){
        // handle_number_key
        return false;
    }

    // lowercase all capitals:
    if(('A' <= key) && ( key <= 'Z')){
        key += ('a' - 'A');
    }

    if(('a' <= key) && ( key <= 'z')){
        switch(key){
            case 'a':
            case 'h':
                renderer.set_key_command(key);
                return true;
            default:
                renderer.set_key_command(key);
                return true;
        }

        mvprintw(4,0, "::set key command(%c); return true; update;", key);
        
        return true;
    }

    return false;
}

void CursesInputHandler::shutdownCurses(){
    // End curses mode
    endwin();
    
    fprintf(stderr, "Program finished: shutting down NCurses.\n\n");
}

void CursesInputHandler::update(bool changed){
    if(changed){
        renderer.update();
    }
}

