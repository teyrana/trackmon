#pragma once

#include <functional>
#include <string>
#include <vector>

#include "core/track-cache.hpp"

#include "ui/display-column.hpp"


namespace ui {
namespace console {

class ConsoleRenderer
{
    public:
        ConsoleRenderer() = delete;

        ConsoleRenderer(TrackCache& cache);

        ~ConsoleRenderer() = default;

        void configure();

        bool paused() const;

        void render();


    private:

        void render_column_headers();
        void render_column_contents();
        void render_horizontal_rule();

        bool toggle_pause();


    private:
        constexpr static size_t max_render_width = 176;
        constexpr static char prefix[] = "    ";

        TrackCache& cache;

        // note: iteration order here defines render order (left to right)
        std::vector<std::reference_wrapper<const DisplayColumn>> columns;

        // render options
        bool render_live;

};

}  // namespace console
}  // namespace ui
