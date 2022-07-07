#pragma once

#include <memory>
#include <string>
#include <vector>

#include "core/track-cache.hpp"

#include "ui/display-column.hpp"


namespace ui {
namespace curses {

class CursesRenderer
{
    public:
        CursesRenderer() = delete;

        CursesRenderer( TrackCache& cache);

        ~CursesRenderer() = default;

        void configure();

        bool paused() const;

        void set_key_command(const char command);
        void set_key_result(const char* _result, size_t length );

        bool toggle_pause();
        bool toggle_help(); 

        void render();


    private:
        void render_command();
        void render_options();
        void render_status_bar();
        void render_column_headers();
        void render_column_contents();

        // void render_status_bar();

    private: 
        static const int header_line_offset = 2;
        
        static const int footer_expand_line_offset = -4;
        static const int footer_minimal_line_offset = -2;
        static const int option_upper_line_offset = -3;
        static const int option_lower_line_offset = -2;
        static const int status_line_offset = -1;

        TrackCache& cache;
        std::vector<const DisplayColumn*> columns;
        char command_key;
        constexpr static size_t command_result_buffer_length = 128;
        char command_result[command_result_buffer_length];

        // render options
        bool render_live;
        bool render_help;

};

}  // namespace curses
}  // namespace ui
