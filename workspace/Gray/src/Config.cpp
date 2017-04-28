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
    seed_set(false),
    run_viewer_flag(true),
    run_physics_flag(true),
    format_hits_set(false),
    format_singles_set(false),
    format_hits(Output::FULL_ASCII),
    format_singles(Output::FULL_ASCII),
    log_nuclear_decays(false),
    log_nonsensitive(false),
    log_nointeraction(false),
    log_errors(false),
    log_all(false),
    time(0),
    start_time(0),
    time_set(false),
    start_time_set(false)
{

}

bool Config::ProcessCommandLine(int argc, char **argv)
{
    if (argc == 1) {
        return(false);
    }

    char * include_cstr = getenv ("GRAY_INCLUDE");
    if (include_cstr) {
        gray_include_env = string(include_cstr);
    }

    materials_filename = gray_include_env + "/Gray_Materials.txt";


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
            seed_set = true;
        } else if (argument == "-f") {
            filename_scene = following_argument;
        } else if (argument == "-p") {
            filename_pipeline = following_argument;
        } else if (argument == "-m") {
            filename_mapping = following_argument;
        } else if (argument == "-i") {
            filename_hits = following_argument;
        } else if (argument == "-o") {
            filename_singles = following_argument;
        } else if (argument == "-t") {
            if ((follow_arg_ss >> time).fail()) {
                cerr << "Invalid time: " << following_argument << endl;
                return(-1);
            }
            time_set = true;
        } else if (argument == "--start") {
            if ((follow_arg_ss >> start_time).fail()) {
                cerr << "Invalid start time: " << following_argument << endl;
                return(-1);
            }
            start_time_set = true;
        } else if (argument == "--mat") {
            materials_filename = following_argument;
        }
    }
    if (filename_scene == "") {
        cerr << "Error: input filename not set" << endl;
        return(false);
    }
    if ((filename_mapping == "") != (filename_pipeline == "")) {
        cerr << "Error: mapping and pipeline filenames both not set" << endl;
        return(false);
    }

    return(true);
}

bool Config::get_log_hits() {
    return(!(filename_hits == ""));
}

bool Config::get_log_singles() {
    return(!(filename_singles == ""));
}

void Config::usage() {
    cout << "Gray (-hb) -f [Scene Description] -o [Output Filename]\n"
    << "  -b : batch mode (don't view the geometry)\n"
    << "  -d : don't run the physics simulation\n"
    << "  -h : print help message\n"
    << "  -i [filename] : set the output for the hits file\n"
    << "  -o [filename] : set the output for the singles file\n"
    << "  -s : set the seed for the rand number generator\n"
    << "  -t : set length of time in for the simulation in seconds\n"
    << "  --start : set the start time in seconds\n"
    << "  --mat : set Gray Materials file. default $GRAY_INCLUDE/Gray_Materials.txt\n"
    << endl;
}


void Config::set_filename_scene(const std::string & name) {
    if (filename_scene == "") {
        filename_scene = name;
    }
}

std::string Config::get_filename_scene() {
    return(filename_scene);
}

void Config::set_filename_pipeline(const std::string & name) {
    if (filename_pipeline == "") {
        filename_pipeline = name;
    }
}

std::string Config::get_filename_pipeline() {
    return(filename_pipeline);
}

void Config::set_filename_mapping(const std::string & name) {
    if (filename_mapping == "") {
        filename_mapping = name;
    }
}

std::string Config::get_filename_mapping() {
    return(filename_mapping);
}

void Config::set_filename_hits(const std::string & name) {
    if (filename_hits == "") {
        filename_hits = name;
    }
}

std::string Config::get_filename_hits() {
    return(filename_hits);
}

void Config::set_filename_singles(const std::string & name) {
    if (filename_singles == "") {
        filename_singles = name;
    }
}

std::string Config::get_filename_singles() {
    return(filename_singles);
}

void Config::set_seed(unsigned long val) {
    if (!seed_set) {
        seed = val;
        seed_set = true;
    }
}

unsigned long Config::get_seed() {
    return(seed);
}

bool Config::get_seed_set() {
    return(seed_set);
}

void Config::set_format_hits(Output::Format fmt) {
    if (!format_hits_set) {
        format_hits = fmt;
        format_hits_set = true;
    }
}

Output::Format Config::get_format_hits() {
    return(format_hits);
}

void Config::set_format_singles(Output::Format fmt) {
    if (!format_singles_set) {
        format_singles = fmt;
        format_singles_set = true;
    }
}

Output::Format Config::get_format_singles() {
    return(format_singles);
}

bool Config::get_run_viewer() {
    return(run_viewer_flag);
}

bool Config::get_run_physics() {
    return(run_physics_flag);
}

void Config::add_pipeline_line(const std::string & line) {
    pipeline_config_lines.push_back(line);
}

std::vector<std::string> Config::get_pipeline_lines() {
    return(pipeline_config_lines);
}

void Config::set_log_nuclear_decays(bool val) {
    log_nuclear_decays = val;
}

void Config::set_log_nonsensitive(bool val) {
    log_nonsensitive = val;
}

void Config::set_log_nointeraction(bool val) {
    log_nointeraction = val;
}

void Config::set_log_errors(bool val) {
    log_errors = val;
}

void Config::set_log_all(bool val) {
    log_all = val;
}

bool Config::get_log_nuclear_decays() {
    return(log_nuclear_decays || log_all);
}

bool Config::get_log_nonsensitive() {
    return(log_nonsensitive || log_all);
}

bool Config::get_log_nointeraction() {
    return(log_nointeraction || log_all);
}

bool Config::get_log_errors() {
    return(log_errors || log_all);
}

bool Config::get_log_all() {
    return(log_all);
}

void Config::set_time(double val) {
    if (!time_set) {
        time = val;
        time_set = true;
    }
}

double Config::get_time() const {
    return(time);
}

void Config::set_start_time(double val) {
    if (!start_time_set) {
        start_time = val;
        start_time_set = true;
    }
}

double Config::get_start_time() const {
    return(start_time);
}

std::string Config::get_materials_filename() const {
    return(materials_filename);
}
