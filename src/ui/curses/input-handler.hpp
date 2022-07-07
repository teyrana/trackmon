#pragma once

#include <chrono>
#include <string>

#include "renderer.hpp"



namespace ui {
namespace curses {

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

}  // namespace curses
}  // namespace ui
