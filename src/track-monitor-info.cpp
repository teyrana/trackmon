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

#include <cstdio>
#include <cstdlib>

#include "track-monitor-info.hpp"

void showExampleConfigAndExit(){
    printf("\n");
    printf("================================================================\n");
    printf(" uCensus example MOOS Configuration \n");
    printf("================================================================\n");
    printf("\n");
    printf("ProcessConfig = uCTrackMonitor\n");
    printf("{\n");
    printf("AppTick    = 4\n");
    printf("CommsTick  = 4\n");
    printf("   // -- no configuration -- \n");
    printf("\n");
    printf("}\n");
    exit(0);
}

void showHelpAndExit(){
    printf("\n");
    printf("\n");
    printf("================================================================\n");
    printf("Usage: uCTrackMonitor <file.moos> [options]\n");
    printf("================================================================\n");
    showSynopsis();
    printf("\n");
    printf("Options:\n");
    printf("  <None>\n");
    // printf("  --version,-v\n");
    // printf("    Display the release version of uXMS.\n");
    printf("\n");

    exit(0);
}

void showInterfaceAndExit(){
    printf("\n");
    printf("================================================================\n");
    printf(" uCTrackMonitor\n");
    printf("================================================================\n");
    showSynopsis();
    printf("\n");
    printf("Subscriptions\n");
    printf("-------------------------------------\n");
    printf("  NODE_REPORT       = Used to listen to all NODE_REPORT that this community knows about.");
    printf("  NODE_REPORT_LOCAL = Monitored for the current vessel's track.");
    printf("\n");
    printf("Publications\n");
    printf("-------------------------------------\n");
    printf("  -- None --\n");
    printf("\n");
    printf("\n");

    exit(0);
}

void showSynopsis(){
    printf("Synopsis\n");
    printf("-------------------------------------\n");
    printf("  This app monitors track traffic, and provides a dynamic, configurable way to inspect same.\n");
    printf("  In-app commands are displayed in the screen footer.\n");
}

void showVersionAndExit(){
    printf("uCensus - Version 0.0.1\n");
    exit(0);
}
