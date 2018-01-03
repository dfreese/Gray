#include <fstream>
#include <iostream>
#include <Gray/Config.h>
#include <Gray/LoadDetector.h>
#include <Output/BinaryFormat.h>
#include <Output/Input.h>
#include <Output/Output.h>
#include <Physics/Interaction.h>
#include <Daq/InteractionStream.h>
#include <Random/Random.h>

using namespace std;

int main(int argc, char ** argv) {
    Config config;
    int config_status = config.ProcessCommandLine(argc, argv, false);
    if (config_status < 0) {
        Config::usage();
        return(1);
    } else if (config_status > 0) {
        Config::usage();
        return(0);
    }

    if (!config.get_filename_scene().empty()) {
        if (!LoadDetector::LoadConfig(config.get_filename_scene(), config)) {
            cerr << "Error loading from scene/config file"
                 << config.get_filename_scene() << endl;
            return(2);
        }
    }

    if (config.get_seed_set()) {
        Random::Seed(config.get_seed());
    } else {
        Random::Seed();
    }

    if (config.get_filename_hits().empty()) {
        cerr << "Filename not specified" << endl;
        return(2);
    }

    if (!config.get_format_hits_set()) {
        cerr << "Input format not specified" << endl;
        return(2);
    }
    // The config class protects the variables from being overwritten, so call
    // set functions to make sure something is set.
    config.set_format_singles(config.get_format_hits());
    config.set_format_coinc(config.get_format_hits());

    if (config.get_verbose()) {
        cout << "input filename   : " << config.get_filename_hits() << endl;
        cout << "singles filename : " << config.get_filename_singles() << endl;
        cout << "map filename     : " << config.get_filename_mapping() << endl;
        cout << "process filename : " << config.get_filename_process() << endl;
        cout << "coinc filenames  :";
        for (const auto & name: config.get_filenames_coinc()) {
            cout << " " << name;
        }
        cout << endl;
        cout << "seed : " << config.get_seed() << endl;
        cout << "input format   : " << config.get_format_hits() << endl;
        cout << "singles format : " << config.get_format_singles() << endl;
        cout << "coinc format   : " << config.get_format_coinc() << endl;
    }

    Input input;
    input.set_format(config.get_format_hits());
    // Assume the variable format mask will be picked up from the file header.
    if (!input.set_logfile(config.get_filename_hits())) {
        cerr << "Opening input failed" << endl;
        return(4);
    }
    config.set_singles_var_output_write_flags(input.get_write_flags());
    config.set_coinc_var_output_write_flags(input.get_write_flags());

    InteractionStream singles_stream(config.get_sort_time());
    int no_detectors = singles_stream.load_mappings(config.get_filename_mapping());
    if (no_detectors < 0) {
        cerr << "Loading mapping file failed" << endl;
        return(2);
    }

    if (config.get_filename_process().empty()) {
        vector<string> proc_lines = config.get_process_lines();
        if (proc_lines.empty()) {
            cerr << "No process steps specified" << endl;
            return(3);
        }
        int proc_load_status = singles_stream.set_processes(proc_lines);
        if (proc_load_status < 0) {
            cerr << "Loading process lines failed" << endl;
            return(2);
        }
    } else {
        int proc_load_status = singles_stream.load_processes(config.get_filename_process());
        if (proc_load_status < 0) {
            cerr << "Loading process file failed" << endl;
            return(2);
        }
    }

    if (!config.get_log_coinc() && !config.get_log_singles()) {
        cerr << "No output specified" << endl;
        return(7);
    }


    Output output;
    if (config.get_log_singles()) {
    output.SetFormat(config.get_format_singles());
        output.SetVariableOutputMask(config.get_singles_var_output_write_flags());
        if (!output.SetLogfile(config.get_filename_singles())) {
            cerr << "Failed to open output file: "
            << config.get_filename_singles() << endl;
            return(5);
        }
    }

    std::vector<Output> outputs_coinc(singles_stream.no_coinc_processes());
    if (config.get_log_coinc()) {
        if (singles_stream.no_coinc_processes() != config.get_no_coinc_filenames()) {
            cerr << "Incorrect number of filenames specified for coinc outputs"
            << endl;
            return(4);
        }
        for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
            Output & output_coinc = outputs_coinc[idx];
            output_coinc.SetFormat(config.get_format_coinc());
            output_coinc.SetVariableOutputMask(config.get_coinc_var_output_write_flags());
            output_coinc.SetLogfile(config.get_filename_coinc(idx));
        }
    }

    while (input.read_interactions(singles_stream.get_buffer(), 100000)) {
        singles_stream.process_singles();
        if (config.get_log_singles()) {
            output.LogSingles(singles_stream.singles_begin(),
                              singles_stream.singles_end());
        }

        for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
            singles_stream.process_coinc(idx);
            if (config.get_log_coinc()) {
                outputs_coinc[idx].LogCoinc(singles_stream.coinc_begin(),
                                            singles_stream.coinc_end(),
                                            true);
            }
        }
        singles_stream.clear_complete();
    }

    singles_stream.stop_singles();
    if (config.get_log_singles()) {
        output.LogSingles(singles_stream.singles_begin(), singles_stream.singles_end());
    }
    
    for (size_t idx = 0; idx < singles_stream.no_coinc_processes(); idx++) {
        singles_stream.stop_coinc(idx);
        if (config.get_log_coinc()) {
            outputs_coinc[idx].LogCoinc(singles_stream.coinc_begin(),
                                        singles_stream.coinc_end(), true);
        }
    }

    if (config.get_verbose()) {
        cout << "______________\n DAQ Stats\n______________\n"
             << singles_stream << endl;
    }
    return(0);
}
