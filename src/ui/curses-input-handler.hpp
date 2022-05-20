#pragma once

#include <chrono>
#include <string>
// #include <memory>

#include "curses-renderer.hpp"

using std::string;

class CursesInputHandler
{
    public:
        CursesInputHandler() = delete;
        
        CursesInputHandler(TrackCache& cache);

        ~CursesInputHandler() =  default;

        std::chrono::system_clock::time_point update( bool changed );


    private:
        void configure();

        bool handle_input();

        void shutdownCurses();

    private:
        CursesRenderer renderer;
        std::chrono::system_clock::time_point last_update_;

};
