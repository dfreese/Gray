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

    std::string filename_scene;
    std::string filename_pipeline;
    std::string filename_hits;
    std::string filename_output;
    unsigned long seed;
    bool seed_set;
    bool run_viewer_flag;
    bool run_physics_flag;
    bool log_hits;
    bool log_singles;
};

#endif /* Config_h */
