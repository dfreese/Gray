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
    void add_pipeline_line(const std::string & line);
    std::vector<std::string> get_pipeline_lines();
    void set_log_nuclear_decays(bool val);
    void set_log_nonsensitive(bool val);
    void set_log_nointeraction(bool val);
    void set_log_errors(bool val);
    void set_log_all(bool val);
    bool get_log_nuclear_decays();
    bool get_log_nonsensitive();
    bool get_log_nointeraction();
    bool get_log_errors();
    bool get_log_all();
    void set_time(double val);
    double get_time() const;
    void set_start_time(double val);
    double get_start_time() const;
    std::string get_materials_filename() const;
    std::string get_isotopes_filename() const;
    bool set_hits_var_output_write_flags(const std::string & mask);
    Output::WriteFlags get_hits_var_output_write_flags() const;
    bool set_singles_var_output_write_flags(const std::string & mask);
    Output::WriteFlags get_singles_var_output_write_flags() const;

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
    bool log_nuclear_decays;
    bool log_nonsensitive;
    bool log_nointeraction;
    bool log_errors;
    bool log_all;
    std::vector<std::string> pipeline_config_lines;
    double time;
    double start_time;
    bool time_set;
    bool start_time_set;
    std::string gray_include_env;
    std::string materials_filename;
    std::string isotopes_filename;
    Output::WriteFlags hits_var_output_write_flags;
    Output::WriteFlags singles_var_output_write_flags;
};

#endif /* Config_h */
