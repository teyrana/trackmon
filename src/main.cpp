#include <string>
#include <vector>

#ifdef ENABLE_MOOS_APP
#include "MBUtils.h"
#endif //ENABLE_MOOS_APP

#include "track-monitor-info.hpp"
#include "track-monitor.hpp"


using std::string;

int main(int argc, char *argv[])
{
    string mission_file;
    string run_command("uCensus");
    string incoming_var;
    string outgoing_var;

    for(int i=1; i<argc; i++) {
        string argi = argv[i];

        if((argi=="-e") || (argi=="--example") || (argi=="-example")){
            showExampleConfigAndExit();
        }else if((argi == "-h") || (argi == "--help") || (argi=="-help")){
            showHelpAndExit();
        }else if((argi == "-i") || (argi == "--interface")){
            showInterfaceAndExit();
        }else if((argi == "-v") || (argi == "--version")){
            showVersionAndExit();

        // add more arguments here.

        }else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++")){
            mission_file = argi;
        }else if(i==2){
            run_command = argi;
        }
    }

    if(mission_file == "")
        showHelpAndExit();

#ifdef ENABLE_MOOS
    TrackMonitor monitor; 

    monitor.Run(run_command.c_str(), mission_file.c_str());

    fprintf(stderr, "MOOS Client Finished.\n");
#endif // ENABLE_MOOS

    return EXIT_SUCCESS;
}
