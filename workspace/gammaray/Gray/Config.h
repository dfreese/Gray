//
//  Config.h
//  Gray
//
//  Created by David Freese on 2/17/17.
//
//

#ifndef Config_h
#define Config_h

#include <vector>
#include <string>
#include <Output/Output.h>

class Config {
public:
    Config() = default;
    int ProcessCommandLine(int argc, char **argv, bool fail_without_scene);
    static void usage();
    static void version();

    void set_filename_scene(const std::string & name);
    std::string get_filename_scene() const;
    void set_filename_process(const std::string & name);
    std::string get_filename_process() const;
    void set_filename_mapping(const std::string & name);
    std::string get_filename_mapping() const;
    void set_filename_hits(const std::string & name);
    std::string get_filename_hits() const;
    void set_filename_singles(const std::string & name);
    std::string get_filename_singles() const;
    void add_filename_coinc(const std::string & name);
    void set_filename_coinc(size_t idx, const std::string & name);
    std::string get_filename_coinc(size_t idx) const;
    size_t get_no_coinc_filenames() const;
    const std::vector<std::string> & get_filenames_coinc() const;
    void set_seed(unsigned long val);
    unsigned long get_seed() const;
    bool set_format(const std::string & fmt);
    bool set_format_hits(const std::string & fmt);
    void set_format_hits(const Output::Format & fmt);
    Output::Format get_format_hits() const;
    bool get_format_hits_set() const;
    bool set_format_singles(const std::string & fmt);
    void set_format_singles(const Output::Format & fmt);
    Output::Format get_format_singles() const;
    bool set_format_coinc(const std::string & fmt);
    void set_format_coinc(const Output::Format & fmt);
    Output::Format get_format_coinc() const;
    bool get_log_hits() const;
    bool get_log_singles() const;
    bool get_log_coinc() const;
    bool get_log_any() const;
    void add_process_line(const std::string & line);
    std::vector<std::string> get_process_lines() const;
    void set_log_nondepositing_inter(bool val);
    void set_log_nuclear_decays(bool val);
    void set_log_nonsensitive(bool val);
    void set_log_nointeraction(bool val);
    void set_log_errors(bool val);
    void set_log_all(bool val);
    bool get_log_nondepositing_inter() const;
    bool get_log_nuclear_decays() const;
    bool get_log_nonsensitive() const;
    bool get_log_nointeraction() const;
    bool get_log_errors() const;
    bool get_log_all() const;
    void set_time(double val);
    double get_time() const;
    void set_start_time(double val);
    double get_start_time() const;
    void set_sort_time(double val);
    double get_sort_time() const;
    std::string get_materials_filename() const;
    std::string get_isotopes_filename() const;
    std::string get_physics_filename() const;
    bool set_hits_var_output_write_flags(const std::string & mask);
    void set_hits_var_output_write_flags(const Output::WriteFlags & mask);
    Output::WriteFlags get_hits_var_output_write_flags() const;
    bool set_singles_var_output_write_flags(const std::string & mask);
    void set_singles_var_output_write_flags(const Output::WriteFlags & mask);
    Output::WriteFlags get_singles_var_output_write_flags() const;
    bool set_coinc_var_output_write_flags(const std::string & mask);
    void set_coinc_var_output_write_flags(const Output::WriteFlags & mask);
    Output::WriteFlags get_coinc_var_output_write_flags() const;
    bool get_verbose() const;
    bool get_run_overlap_test() const;
    bool get_write_pos() const;
    bool get_write_map() const;
    std::string get_write_pos_filename() const;
    std::string get_write_map_filename() const;
    int get_no_threads() const;

private:
    std::string filename_scene;
    std::string filename_process;
    std::string filename_mapping;
    std::string filename_hits;
    std::string filename_singles;
    std::vector<std::string> filenames_coinc;
    unsigned long seed = 0;
    bool seed_set_command_line = false;
    bool format_hits_set = false;
    bool format_singles_set = false;
    bool format_coinc_set = false;
    Output::Format format_hits = Output::VARIABLE_ASCII;
    Output::Format format_singles = Output::VARIABLE_ASCII;
    Output::Format format_coinc = Output::VARIABLE_ASCII;
    bool log_nondepositing_inter = false;
    bool log_nuclear_decays = false;
    bool log_nonsensitive = false;
    bool log_nointeraction = false;
    bool log_errors = false;
    bool log_all = false;
    std::vector<std::string> process_lines;
    double time = 0;
    bool time_set = false;
    double start_time = 0;
    bool start_time_set = false;
    double sort_time = -1;
    bool sort_time_set = false;
    std::string gray_include_env;
    std::string physics_filename;
    Output::WriteFlags hits_var_output_write_flags;
    Output::WriteFlags singles_var_output_write_flags;
    Output::WriteFlags coinc_var_output_write_flags;
    bool hits_var_output_write_flags_set = false;
    bool singles_var_output_write_flags_set = false;
    bool coinc_var_output_write_flags_set = false;
    bool verbose = false;
    bool run_overlap_test = false;
    std::string write_pos_filename = "";
    std::string write_map_filename = "";
    int no_threads = 1;
};

#endif /* Config_h */
