/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <fstream>
#include <iostream>
#include <Daq/Mapping.h>
#include <Daq/DaqModel.h>
#include <Gray/Config.h>
#include <Gray/LoadDetector.h>
#include <Output/Input.h>
#include <Output/Output.h>
#include <Physics/Interaction.h>
#include <Random/Random.h>

using namespace std;

int main(int argc, char ** argv) {
    Config config;
    int config_status = config.ProcessCommandLine(argc, argv, false);
    if (config_status < 0) {
        return(1);
    } else if (config_status > 0) {
        return(0);
    }

    if (!config.get_filename_scene().empty()) {
        if (!LoadDetector::LoadConfig(config.get_filename_scene(), config)) {
            cerr << "Error loading from scene/config file"
                 << config.get_filename_scene() << endl;
            return(2);
        }
    }

    Random::SetSeed(config.get_seed());

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

    DaqModel daq_model(config.get_sort_time());
    Mapping::IdMappingT mapping;
    if (!Mapping::LoadMapping(config.get_filename_mapping(), mapping)) {
        cerr << "Loading mapping file failed" << endl;
        return(2);
    }

    if (config.get_filename_process().empty()) {
        vector<string> proc_lines = config.get_process_lines();
        if (proc_lines.empty()) {
            cerr << "No process steps specified" << endl;
            return(3);
        }
        int proc_load_status = daq_model.set_processes(proc_lines, mapping);
        if (proc_load_status < 0) {
            cerr << "Loading process lines failed" << endl;
            return(2);
        }
    } else {
        int proc_load_status = daq_model.load_processes(
                config.get_filename_process(), mapping);
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
        if (!output.SetLogfile(config.get_filename_singles(), /*write_header=*/true)) {
            cerr << "Failed to open output file: "
            << config.get_filename_singles() << endl;
            return(5);
        }
    }

    std::vector<Output> outputs_coinc(daq_model.no_coinc_processes());
    if (config.get_log_coinc()) {
        if (daq_model.no_coinc_processes() != config.get_no_coinc_filenames()) {
            cerr << "Incorrect number of filenames specified for coinc outputs"
            << endl;
            return(4);
        }
        for (size_t idx = 0; idx < daq_model.no_coinc_processes(); idx++) {
            Output & output_coinc = outputs_coinc[idx];
            output_coinc.SetFormat(config.get_format_coinc());
            output_coinc.SetVariableOutputMask(config.get_coinc_var_output_write_flags());
            output_coinc.SetLogfile(config.get_filename_coinc(idx), /*write_header=*/true);
        }
    }

    while (input.read_interactions(daq_model.get_buffer(), 100000)) {
        daq_model.process_singles();
        if (config.get_log_singles()) {
            output.LogSingles(daq_model.singles_begin(),
                              daq_model.singles_end());
        }

        for (size_t idx = 0; idx < daq_model.no_coinc_processes(); idx++) {
            daq_model.process_coinc(idx);
            if (config.get_log_coinc()) {
                outputs_coinc[idx].LogCoinc(daq_model.coinc_begin(),
                                            daq_model.coinc_end(),
                                            true);
            }
        }
        daq_model.clear_complete();
    }

    daq_model.stop_singles();
    if (config.get_log_singles()) {
        output.LogSingles(daq_model.singles_begin(), daq_model.singles_end());
    }

    for (size_t idx = 0; idx < daq_model.no_coinc_processes(); idx++) {
        daq_model.stop_coinc(idx);
        if (config.get_log_coinc()) {
            outputs_coinc[idx].LogCoinc(daq_model.coinc_begin(),
                                        daq_model.coinc_end(), true);
        }
    }

    if (config.get_verbose()) {
        cout << "______________\n DAQ Stats\n______________\n"
             << daq_model << endl;
    }
    return(0);
}
