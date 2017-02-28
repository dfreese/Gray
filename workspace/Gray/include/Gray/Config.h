//
//  Config.h
//  Gray
//
//  Created by David Freese on 2/17/17.
//
//

#ifndef Config_h
#define Config_h

#include <string>

class Config {
public:
    Config();
    bool ProcessCommandLine(int argc, char **argv);
    static void usage();

    std::string filename_detector;
    std::string filename_output;
    unsigned long seed;
    bool run_viewer_flag;
    bool run_physics_flag;
};

#endif /* Config_h */
