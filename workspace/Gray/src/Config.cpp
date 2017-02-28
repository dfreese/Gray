//
//  Config.cpp
//  Gray
//
//  Created by David Freese on 2/17/17.
//
//

#include <Gray/Config.h>
#include <sstream>
#include <iostream>

using namespace std;

Config::Config() :
    filename_detector(""),
    filename_output(""),
    seed(0),
    run_viewer_flag(true),
    run_physics_flag(true)
{

}

bool Config::ProcessCommandLine(int argc, char **argv)
{
    if (argc == 1) {
        return(false);
    }

    bool setFilenameDetector = false;
    bool setFilenameOutput = false;

    for (int ix = 1; ix < argc; ix++) {
        string argument(argv[ix]);
        if (ix < (argc - 1)) {
            // Arguments requiring an input
            string following_argument(argv[ix + 1]);
            stringstream follow_arg_ss;
            follow_arg_ss << following_argument;
            if (argument == "-seed" || (argument == "-s")) {
                if (!(follow_arg_ss >> seed)) {
                    cerr << "Loading high energy threshold failed." << endl;
                    return(-1);
                }
                ix++; continue;
            }
        }
        if (argument == "-h" || argument == "--help") {
            return(false);
        } else if (argument == "-b") {
            run_viewer_flag = false;
        } else if (argument == "-d") {
            run_physics_flag = false;
        } else {
            if (setFilenameDetector == false) {
                filename_detector = argument;
                setFilenameDetector = true;
            } else if (setFilenameOutput == false) {
                filename_output = argument;
                setFilenameOutput = true;
            }
        }
    }
    if (!setFilenameDetector) {
        cerr << "Error: input filename not set" << endl;
        return(false);
    } else if (!setFilenameOutput) {
        filename_output = filename_detector + ".dat";
    }
    
    return true;
}


void Config::usage() {
    cout << "Gray (-hb) [Scene Description] [Output Filename]\n"
    << "  -b : batch mode (don't view the geometry)\n"
    << "  -d : don't run the physics simulation\n"
    << "  -h : print help message\n"
    << "  -s [seed] : set the seed for the rand number generator\n"
    << endl;
}
