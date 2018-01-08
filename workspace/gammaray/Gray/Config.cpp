//
//  Config.cpp
//  Gray
//
//  Created by David Freese on 2/17/17.
//
//

#include <Gray/Config.h>
#include <ctime>
#include <sstream>
#include <iostream>
#include <Gray/Mpi.h>
#include <Math/Math.h>
#include <Version/Version.h>

using namespace std;

/*
 * Zero indicates success
 * Less than 0 indicates fatal error
 * Greater than 0 indicates non-fatal exit required (i.e. help).
 */
int Config::ProcessCommandLine(int argc, char **argv, bool fail_without_scene)
{
    if (argc == 1) {
        Config::usage();
        return(1);
    }
    Mpi::Init(argc, argv);
    // Set the default seed to be the current time.
    seed = Math::hash(std::time(NULL));

    char * include_cstr = getenv ("GRAY_INCLUDE");
    if (include_cstr) {
        // TODO: use portable method.
        gray_include_env = string(include_cstr) + "/";
    }

    physics_filename = gray_include_env + "GrayPhysics.json";

    // Arguments not requiring an input
    for (int ix = 1; ix < argc; ix++) {
        string argument(argv[ix]);
        if ((argument == "--help") || (argument == "-h")) {
            Config::usage();
            return (1);
        }
        if (argument == "--version") {
            Config::version();
            return (2);
        }
        if (argument == "--test_overlap") {
            run_overlap_test = true;
        }
        if (argument == "-v") {
            verbose = true;
        }
    }

    // Arguments requiring an input
    for (int ix = 1; ix < (argc - 1); ix++) {
        string argument(argv[ix]);
        string following_argument(argv[ix + 1]);
        stringstream follow_arg_ss(following_argument);
        if (argument == "--seed") {
            unsigned int tmp_seed;
            if (!(follow_arg_ss >> tmp_seed)) {
                cerr << "Invalid seed: " << following_argument << endl;
                return(-1);
            }
            seed = tmp_seed;
            seed_set_command_line = true;
        } else if (argument == "-f") {
            set_filename_scene(following_argument);
        } else if (argument == "-p") {
            set_filename_process(following_argument);
        } else if (argument == "-m") {
            set_filename_mapping(following_argument);
        } else if (argument == "-i") {
            set_filename_hits(following_argument);
        } else if (argument == "-s") {
            set_filename_singles(following_argument);
        } else if (argument == "-c") {
            add_filename_coinc(following_argument);
        } else if (argument == "-t") {
            double tmp_time;
            if ((follow_arg_ss >> tmp_time).fail()) {
                cerr << "Invalid time: " << following_argument << endl;
                return(-2);
            }
            set_time(tmp_time);
        } else if (argument == "--start") {
            double tmp_start_time;
            if ((follow_arg_ss >> tmp_start_time).fail()) {
                cerr << "Invalid start time: " << following_argument << endl;
                return(-3);
            }
            set_start_time(tmp_start_time);
        } else if (argument == "--phys") {
            physics_filename = following_argument;
        } else if (argument == "--fmt") {
            if (!set_format(following_argument)) {
                cerr << "Invalid format: " << following_argument << "\n";
                return(-4);
            }
        } else if (argument == "--hits_fmt") {
            if (!set_format_hits(following_argument)) {
                cerr << "Invalid hits format: " << following_argument << endl;
                return(-4);
            }
        } else if (argument == "--singles_fmt") {
            if (!set_format_singles(following_argument)) {
                cerr << "Invalid singles format: " << following_argument << endl;
                return(-5);
            }
        } else if (argument == "--coinc_fmt") {
            if (!set_format_coinc(following_argument)) {
                cerr << "Invalid coinc format: " << following_argument << endl;
                return(-6);
            }
        } else if (argument == "--hits_mask") {
            if (!set_hits_var_output_write_flags(following_argument)) {
                cerr << "Invalid hits mask: " << following_argument << endl;
                return(-7);
            }
        } else if (argument == "--singles_mask") {
            if (!set_singles_var_output_write_flags(following_argument)) {
                cerr << "Invalid singles mask: " << following_argument << endl;
                return(-8);
            }
        } else if (argument == "--coinc_mask") {
            if (!set_coinc_var_output_write_flags(following_argument)) {
                cerr << "Invalid coinc mask: " << following_argument << endl;
                return(-9);
            }
        } else if (argument == "--sort") {
            double tmp_sort_time;
            if ((follow_arg_ss >> tmp_sort_time).fail()) {
                cerr << "Invalid sort time: " << following_argument << endl;
                return(-10);
            }
            set_sort_time(tmp_sort_time);
        } else if (argument == "--write_pos") {
            write_pos_filename = following_argument;
        } else if (argument == "--write_map") {
            write_map_filename = following_argument;
        } else if (argument == "-nt") {
            if ((follow_arg_ss >> no_threads).fail()) {
                cerr << "Invalid number of threads: " << following_argument << endl;
                return(-11);
            }
        } else if (argument.front() == '-') {
            cerr << "Unrecognized command line argument: " << argument << "\n";
            return (-99);
        }
    }

    if (fail_without_scene && get_filename_scene().empty()) {
        cerr << "Error: scene filename not set" << endl;
        return(-11);
    }

    return(0);
}

bool Config::get_log_hits() const {
    return(!(filename_hits == ""));
}

bool Config::get_log_singles() const {
    return(!(filename_singles == ""));
}

bool Config::get_log_coinc() const {
    return(!filenames_coinc.empty());
}

bool Config::get_log_any() const {
    return(get_log_hits() || get_log_singles() || get_log_coinc());
}

void Config::usage() {
    cout << "gray (-v) -f [Scene Description]\n"
    << "  -h or --help : print help message\n"
    << "  --version : print version information\n"
    << "  -i [filename] : set the output hits file / input for gray-daq\n"
    << "  -s [filename] : set the output for the singles file\n"
    << "  -c [filename] : set an output for the coinc files (order matters)\n"
    << "  -p [filename] : set the input process file for daq model\n"
    << "  -m [filename] : set the input mapping file for daq model\n"
    << "  -t [time] : set length of time in for the simulation in seconds\n"
    << "  --seed [seed] : set the seed for the rand number generator\n"
    << "  --start [time] : set the start time in seconds\n"
    << "  --phys [filename] : set Gray Physics file. default=$GRAY_INCLUDE/GrayPhysics.json\n"
    << "  --fmt [type] : all input and output formats: var_ascii\n"
    << "  --hits_fmt [type] : hits output or input format, default: var_ascii\n"
    << "  --singles_fmt [type] : default: var_ascii or input type\n"
    << "  --coinc_fmt [type] : default: var_ascii or input type\n"
    << "  --hits_mask [type] : default: all on, or input mask\n"
    << "  --singles_mask [type] : default: all on, or input mask\n"
    << "  --coinc_mask [type] : default: all on, or input mask\n"
    << "  --test_overlap : run overlap testing for the input geometry\n"
    << "  --write_pos [filename] : write out the detector positions to file\n"
    << "  --write_map [filename] : write out mapping information used to file\n"
    << "  -nt [number] : number of threads to use, default = 1\n"
    << " gray-daq only: \n"
    << "  --sort [time] : sort the incoming events, assuming this max out of order time\n"
    << endl;
}

void Config::version() {
    cout << "Gray: a Ray Tracing-based Monte Carlo Simulator for PET\n"
         << "\n"
         << "  Copyright: Peter D. Olcott and David L. Freese, "
         << Version::CopyrightYear() << "\n"
         << "  Version: "  << Version::VersionStr() << "\n"
         << "  Git Commit: " << Version::GitSHA1() << "\n"
         << "\n";
}

void Config::set_filename_scene(const std::string & name) {
    if (filename_scene == "") {
        filename_scene = name;
    }
}

std::string Config::get_filename_scene() const {
    return(filename_scene);
}

void Config::set_filename_process(const std::string & name) {
    if (filename_process == "") {
        filename_process = name;
    }
}

std::string Config::get_filename_process() const {
    return(filename_process);
}

void Config::set_filename_mapping(const std::string & name) {
    if (filename_mapping == "") {
        filename_mapping = name;
    }
}

std::string Config::get_filename_mapping() const {
    return(filename_mapping);
}

void Config::set_filename_hits(const std::string & name) {
    if (filename_hits == "") {
        filename_hits = name;
    }
}

std::string Config::get_filename_hits() const {
    return(filename_hits);
}

void Config::set_filename_singles(const std::string & name) {
    if (filename_singles == "") {
        filename_singles = name;
    }
}

std::string Config::get_filename_singles() const {
    return(filename_singles);
}

void Config::set_seed(unsigned long val) {
    if (!seed_set_command_line) {
        seed = val;
    }
}

unsigned long Config::get_seed() const {
    return(seed);
}

bool Config::set_format(const std::string & fmt) {
    return (set_format_hits(fmt) &&
            set_format_singles(fmt) &&
            set_format_coinc(fmt));
}

bool Config::set_format_hits(const std::string & fmt) {
    if (format_hits_set) {
        return(true);
    }
    if (Output::ParseFormat(fmt, format_hits) < 0) {
        return(false);
    }
    format_hits_set = true;
    return(true);
}

void Config::set_format_hits(const Output::Format & fmt) {
    if (!format_hits_set) {
        format_hits = fmt;
        format_hits_set = true;
    }
}

Output::Format Config::get_format_hits() const {
    return(format_hits);
}

bool Config::get_format_hits_set() const {
    return(format_hits_set);
}

bool Config::set_format_singles(const std::string & fmt) {
    if (format_singles_set) {
        return(true);
    }
    if (Output::ParseFormat(fmt, format_singles) < 0) {
        return(false);
    }
    format_singles_set = true;
    return(true);
}

void Config::set_format_singles(const Output::Format & fmt) {
    if (!format_singles_set) {
        format_singles = fmt;
        format_singles_set = true;
    }
}

Output::Format Config::get_format_singles() const {
    return(format_singles);
}

bool Config::set_format_coinc(const std::string & fmt) {
    if (format_coinc_set) {
        return(true);
    }
    if (Output::ParseFormat(fmt, format_coinc) < 0) {
        return(false);
    }
    format_coinc_set = true;
    return(true);
}

void Config::set_format_coinc(const Output::Format & fmt) {
    if (!format_coinc_set) {
        format_coinc = fmt;
        format_coinc_set = true;
    }
}

Output::Format Config::get_format_coinc() const {
    return(format_coinc);
}

void Config::add_process_line(const std::string & line) {
    process_lines.push_back(line);
}

std::vector<std::string> Config::get_process_lines() const {
    return(process_lines);
}

void Config::set_log_nondepositing_inter(bool val) {
    log_nondepositing_inter = val;
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

bool Config::get_log_nondepositing_inter() const {
    return(log_nondepositing_inter || log_all);
}

bool Config::get_log_nuclear_decays() const {
    return(log_nuclear_decays || log_all);
}

bool Config::get_log_nonsensitive() const {
    return(log_nonsensitive || log_all);
}

bool Config::get_log_nointeraction() const {
    return(log_nointeraction || log_all);
}

bool Config::get_log_errors() const {
    return(log_errors || log_all);
}

bool Config::get_log_all() const {
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

void Config::set_sort_time(double val) {
    if (!sort_time_set) {
        sort_time = val;
        sort_time_set = true;
    }
}

double Config::get_sort_time() const {
    return(sort_time);
}

std::string Config::get_physics_filename() const {
    return(physics_filename);
}

bool Config::set_hits_var_output_write_flags(const std::string & mask) {
    if (hits_var_output_write_flags_set) {
        return(true);
    } else {
        return(Output::parse_write_flags_mask(hits_var_output_write_flags,
                                              mask));
    }
}

void Config::set_hits_var_output_write_flags(const Output::WriteFlags & mask) {
    if (!hits_var_output_write_flags_set) {
        hits_var_output_write_flags = mask;
        hits_var_output_write_flags_set = true;
    }
}

Output::WriteFlags Config::get_hits_var_output_write_flags() const {
    return(hits_var_output_write_flags);
}

bool Config::set_singles_var_output_write_flags(const std::string & mask) {
    if (singles_var_output_write_flags_set) {
        return(true);
    } else {
        return(Output::parse_write_flags_mask(singles_var_output_write_flags,
                                                   mask));
    }
}

void Config::set_singles_var_output_write_flags(const Output::WriteFlags & mask) {
    if (!singles_var_output_write_flags_set) {
        singles_var_output_write_flags = mask;
        singles_var_output_write_flags_set = true;
    }
}

Output::WriteFlags Config::get_singles_var_output_write_flags() const {
    return(singles_var_output_write_flags);
}


bool Config::set_coinc_var_output_write_flags(const std::string & mask) {
    if (coinc_var_output_write_flags_set) {
        return(true);
    } else {
        return(Output::parse_write_flags_mask(coinc_var_output_write_flags,
                                              mask));
    }
}

void Config::set_coinc_var_output_write_flags(const Output::WriteFlags & mask) {
    if (!coinc_var_output_write_flags_set) {
        coinc_var_output_write_flags = mask;
        coinc_var_output_write_flags_set = true;
    }
}

Output::WriteFlags Config::get_coinc_var_output_write_flags() const {
    return(coinc_var_output_write_flags);
}

void Config::add_filename_coinc(const std::string & name) {
    filenames_coinc.push_back(name);
}

void Config::set_filename_coinc(size_t idx, const std::string & name) {
    if (idx >= filenames_coinc.size()) {
        filenames_coinc.resize(idx + 1);
    }
    filenames_coinc[idx] = name;
}

std::string Config::get_filename_coinc(size_t idx) const {
    return(filenames_coinc.at(idx));
}

size_t Config::get_no_coinc_filenames() const {
    return(filenames_coinc.size());
}

const std::vector<std::string> & Config::get_filenames_coinc() const {
    return(filenames_coinc);
}

bool Config::get_verbose() const {
    return(verbose);
}

bool Config::get_run_overlap_test() const {
    return (run_overlap_test);
}

bool Config::get_write_pos() const {
    return (!write_pos_filename.empty());
}

bool Config::get_write_map() const  {
    return (!write_map_filename.empty());
}

std::string Config::get_write_pos_filename() const {
    return (write_pos_filename);
}

std::string Config::get_write_map_filename() const  {
    return (write_map_filename);
}

int Config::get_no_threads() const {
    return (no_threads);
}
