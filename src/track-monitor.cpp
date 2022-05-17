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

#include <iterator>

#include <ncurses.h>


#include "track-monitor.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#ifdef DEBUG
    FILE* logfile = nullptr;
#endif


//---------------------------------------------------------
// Constructor

TrackMonitor::TrackMonitor()
    : handler(cache)
{
#ifdef DEBUG
    logfile = fopen("debug.log", "w");
#endif
}

TrackMonitor::~TrackMonitor(){
    fprintf(stderr, "closing: TrackMonitor\n");
#ifdef DEBUG
    fflush(logfile);
    fclose(logfile);
#endif
}

//---------------------------------------------------------
// Procedure: OnNewMail

// bool TrackMonitor::OnNewMail( &NewMail)
// {
//     MOOSMSG_LIST::iterator p;
//     for(p = NewMail.begin(); p!=NewMail.end(); p++) {
//         CMOOSMsg &msg = *p;

//         const string key = msg.GetKey();
//         // fprintf(logfile, "    for: %s    from: %s\n", msg.GetKey().c_str(), msg.GetCommunity().c_str());
//         if(("NODE_REPORT"==key) || ("NODE_REPORT_LOCAL"==key)){
//             cache.update(msg.GetAsString());
//         }
//     }

//     // if(changed)
//     //     handler.update(changed);
    
//     return true;
// }


//---------------------------------------------------------
// Procedure: OnConnectToServer

// bool TrackMonitor::OnConnectToServer()
// {
//     // this is the only registration
//     Register("NODE_REPORT", 0.);
//     Register("NODE_REPORT_LOCAL", 0.);

//     fprintf(stderr, "Connected to server!... Initializing Curses:\n");
    
//     // const double app_freq = GetAppFreq();
//     handler.configure();//app_freq);

//     // force an initial draw
//     handler.update(true);

//     return true;
// }

// //---------------------------------------------------------
// bool TrackMonitor::Iterate()
// {
//     handler.handle_input();

//     handler.update(true);

    // unsigned int i, amt = (m_tally_recd - m_tally_sent);
    // for(i=0; i<amt; i++) {
    //     m_tally_sent++;
    //     Notify(m_outgoing_var, m_tally_sent);
    // }
    //
    // // If this is the first iteration just note the start time, otherwise
    // // note the currently calculated frequency and post it to the DB.
    // if(m_start_time_iterations == 0)
    // m_start_time_iterations = MOOSTime();
    // else {
    //     double delta_time = (MOOSTime() - m_start_time_iterations) + 0.01;
    //     double frequency  = (double)(m_iterations) / delta_time;
    //     Notify(m_outgoing_var+"_ITER_HZ", frequency);
    // }
    //
    //
    // // If this is the first time a received msg has been noted, just
    // // note the start time, otherwise calculate and post the frequency.
    // if(amt > 0) {
    //     if(m_start_time_postings == 0)
    //     m_start_time_postings = MOOSTime();
    //     else {
    //         double delta_time = (MOOSTime() - m_start_time_postings) + 0.01;
    //         double frequency = (double)(m_tally_sent) / delta_time;
    //         Notify(m_outgoing_var+"_POST_HZ", frequency);
    //     }
    // }

    
    
//     return(true);
// }


// //---------------------------------------------------------
// // Procedure: OnStartUp()
// //      Note: happens before connection is open
// bool TrackMonitor::OnStartUp()
// {
//     STRING_LIST sParams;
//     STRING_LIST::iterator p;
//     double origin_latitude, origin_longitude;

//     fprintf(logfile, "==== Loading :%s: Config ====\n", GetAppName().c_str() );
//     m_MissionReader.GetConfiguration(GetAppName(), sParams);
//     for(p = sParams.begin();p!=sParams.end();p++) {
//         string line  = *p;
//         string param = tolower(biteStringX(line, '='));
//         string value = line;

//         if("latorigin" == param){
//             origin_latitude = std::atof(value.c_str());
//             fprintf(logfile, "    LatOrigin  == %g\n", origin_latitude );
//         }else if("longorigin" == param){
//             origin_longitude = std::atof(value.c_str());
//             fprintf(logfile, "    LongOrigin == %g\n", origin_longitude );

//         // if("apptick" == param){
//         //     std::cerr << "AppTick == " << value << std::endl;
//         // }else if("commstick"== param){
//         //     std::cerr << "CommsTick == " << value << std::endl;

//         }
//         cache.set_origin(origin_latitude, origin_longitude);
//     }

//     return(true);
// }
