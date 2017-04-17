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
#include <Output/Output.h>

class Config {
public:
    Config();
    bool ProcessCommandLine(int argc, char **argv);
    static void usage();

    void set_filename_scene(const std::string & name);
    std::string get_filename_scene();
    void set_filename_pipeline(const std::string & name);
    std::string get_filename_pipeline();
    void set_filename_mapping(const std::string & name);
    std::string get_filename_mapping();
    void set_filename_hits(const std::string & name);
    std::string get_filename_hits();
    void set_filename_singles(const std::string & name);
    std::string get_filename_singles();
    void set_seed(unsigned long val);
    unsigned long get_seed();
    bool get_seed_set();
    void set_format_hits(Output::Format fmt);
    Output::Format get_format_hits();
    void set_format_singles(Output::Format fmt);
    Output::Format get_format_singles();
    bool get_run_viewer();
    bool get_run_physics();
    bool get_log_hits();
    bool get_log_singles();

private:
    std::string filename_scene;
    std::string filename_pipeline;
    std::string filename_mapping;
    std::string filename_hits;
    std::string filename_singles;
    unsigned long seed;
    bool seed_set;
    bool run_viewer_flag;
    bool run_physics_flag;
    bool format_hits_set;
    bool format_singles_set;
    Output::Format format_hits;
    Output::Format format_singles;
};

#endif /* Config_h */
