//*****************************************************************************
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//*****************************************************************************

#ifndef TRAFFIC_MONITOR_HPP
#define TRAFFIC_MONITOR_HPP

#include <memory>
using std::unique_ptr;

#include "MOOS/libMOOS/MOOSLib.h"

#include "track-cache.hpp"
#include "curses-input-handler.hpp"
#include "curses-renderer.hpp"

// see: http://gobysoft.org/doc/moos/class_c_m_o_o_s_app.html
class TrackMonitor : public CMOOSApp
{
    public:
        TrackMonitor();
        virtual ~TrackMonitor();

        // required / inherited methods
        bool OnNewMail(MOOSMSG_LIST &NewMail) override;
        bool Iterate() override;
        bool OnConnectToServer() override;
        bool OnStartUp() override;

    protected:
        CursesInputHandler handler;
        TrackCache cache;

};

#endif
