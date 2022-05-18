#pragma once

#include <memory>
#include <string>
#include <vector>

#include "display-column.hpp"
#include "track-cache.hpp"

using std::string;

class CursesRenderer
{
    public:
        CursesRenderer() = delete;

        CursesRenderer(TrackCache& cache);

        ~CursesRenderer() = default;

        void configure();

        bool is_paused() const;
        void pause();
        void resume();

        void set_key_command(const char command);

        void update();


    private:
        void render_command();
        void render_options();
        void render_status_bar();
        void render_column_headers();
        void render_column_contents();

        // void render_status_bar();

    private: 
        static const int header_line_offset = 2;
        
        static const int footer_line_offset = -4;
        static const int option_upper_line_offset = -3;
        static const int option_lower_line_offset = -2;
        static const int status_line_offset = -1;

        std::vector<DisplayColumn> columns;
        TrackCache& cache;
        char command;
        bool paused;
};
