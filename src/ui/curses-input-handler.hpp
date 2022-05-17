#pragma once

#include <string>
// #include <memory>

#include "curses-renderer.hpp"

using std::string;

class CursesInputHandler
{
    public:
        CursesInputHandler() = delete;
        
        CursesInputHandler(TrackCache& cache);

        void configure();

        ~CursesInputHandler() =  default;

        bool handle_input();
        
        void update(bool changed);

    private:
        bool handle_option_key(const char key);
        
        void shutdownCurses();

    private:
        CursesRenderer renderer;

};
