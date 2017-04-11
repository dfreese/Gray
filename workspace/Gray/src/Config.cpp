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
    seed(0),
    run_viewer_flag(true),
    run_physics_flag(true),
    log_hits(false),
    log_singles(false)
{

}

bool Config::ProcessCommandLine(int argc, char **argv)
{
    if (argc == 1) {
        return(false);
    }

    // Arguments not requiring an input
    for (int ix = 1; ix < argc; ix++) {
        string argument(argv[ix]);
        if (argument == "-h" || argument == "--help") {
            return(false);
        } else if (argument == "-b") {
            run_viewer_flag = false;
        } else if (argument == "-d") {
            run_physics_flag = false;
        }
    }

    // Arguments requiring an input
    for (int ix = 1; ix < (argc - 1); ix++) {
        string argument(argv[ix]);
        string following_argument(argv[ix + 1]);
        stringstream follow_arg_ss(following_argument);
        if (argument == "-s") {
            if (!(follow_arg_ss >> seed)) {
                cerr << "Invalid seed: " << following_argument << endl;
                return(-1);
            }
        } else if (argument == "-f") {
            filename_scene = following_argument;
        } else if (argument == "-p") {
            filename_pipeline = following_argument;
        } else if (argument == "-i") {
            filename_hits = following_argument;
        } else if (argument == "-o") {
            filename_output = following_argument;
        }
    }
    if (filename_scene == "") {
        cerr << "Error: input filename not set" << endl;
        return(false);
    }

    log_hits = !(filename_hits == "");
    log_singles = !(filename_output == "");
    return(true);
}


void Config::usage() {
    cout << "Gray (-hb) [Scene Description] [Output Filename]\n"
    << "  -b : batch mode (don't view the geometry)\n"
    << "  -d : don't run the physics simulation\n"
    << "  -h : print help message\n"
    << "  -s : set the seed for the rand number generator\n"
    << endl;
}
