/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Gray/Load.h"
#include "Gray/Gray/Config.h"
#include "Gray/Gray/File.h"
#include "Gray/Gray/Syntax.h"

bool Load::ConfigCommand(Command& cmd, Config & config) {
    if (cmd == "hits_format") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for hits_format");
            return (false);
        } else if (!config.set_format_hits(cmd.Join())) {
            cmd.MarkError("Invalid format identifier: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "singles_format") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for singles_format");
            return (false);
        } else if (!config.set_format_singles(cmd.Join())) {
            cmd.MarkError("Invalid format identifier: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "coinc_format") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for coinc_format");
            return (false);
        } else if (!config.set_format_coinc(cmd.Join())) {
            cmd.MarkError("Invalid format identifier: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "hits_output") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for hits_output");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for hits_output");
            return (false);
        }
        config.set_filename_hits(cmd.Join());
        return (true);
    } else if (cmd == "singles_output") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for singles_output");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for singles_output");
            return (false);
        }
        config.set_filename_singles(cmd.Join());
        return (true);
    } else if (cmd == "coinc_output") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for coinc_output");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for coinc_output");
            return (false);
        }
        config.add_filename_coinc(cmd.Join());
        return (true);
    } else if (cmd == "process_file") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for process_file");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for process_file");
            return (false);
        }
        // Make the file relative to whichever file in which the command was
        // placed
        std::string filename = File::Join(File::Dir(cmd.filename), cmd.Join());
        config.set_filename_process(filename);
        return (true);
    }  else if (cmd == "process") {
        if (cmd.tokens.size() == 1) {
            cmd.MarkError("No options given for process");
            return (false);
        }
        // Any line prefaced with pipeilne_config will be processed as a
        // pipeline file.
        config.add_process_line(cmd.Join());
        return (true);
    } else if (cmd == "mapping_file") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for mapping_file");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for mapping_file");
            return (false);
        }
        // Make the file relative to whichever file in which the command was
        // placed
        std::string filename = File::Join(File::Dir(cmd.filename), cmd.Join());
        config.set_filename_mapping(filename);
        return (true);
    } else if (cmd == "hits_var_mask") {
        if (!config.set_hits_var_output_write_flags(cmd.Join())) {
            cmd.MarkError("Invalid variable mask: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "singles_var_mask") {
        if (!config.set_singles_var_output_write_flags(cmd.Join())) {
            cmd.MarkError("Invalid variable mask: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "coinc_var_mask") {
        if (!config.set_coinc_var_output_write_flags(cmd.Join())) {
            cmd.MarkError("Invalid variable mask: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "time") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for time");
            return (false);
        }
        // simulation time in seconds
        double sim_time = 0;
        try {
            sim_time = std::stod(cmd.Join());
        } catch (std::exception& e) {
            cmd.MarkError("Invalid simulation time: " + cmd.Join());
            return (false);
        }
        config.set_time(sim_time);
        return (true);
    } else if (cmd == "start_time") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for start time");
            return (false);
        }
        // time in seconds
        double start_time = 0;
        try {
            start_time = std::stod(cmd.Join());
        } catch (std::exception& e) {
            cmd.MarkError("Invalid start time: " + cmd.Join());
            return (false);
        }
        config.set_start_time(start_time);
        return (true);
    } else if (cmd == "seed") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for seed");
            return (false);
        }
        unsigned long seed = 1;
        try {
            seed = std::stoul(cmd.Join());
        } catch (std::exception& e) {
            cmd.MarkError("Invalid seed: " + cmd.Join());
            return (false);
        }
        config.set_seed(seed);
        return (true);
    } else if (cmd == "log_positron") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_positron: " +
                    cmd.Join());
            return (false);
        }
        config.set_log_nuclear_decays(true);
        return (true);
    } else if (cmd == "log_nondepositing") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_nondepositing: " +
                    cmd.Join());
            return (false);
        }
        config.set_log_nondepositing_inter(true);
        return (true);
    } else if (cmd == "log_nonsensitive") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_nonsensitive: " +
                    cmd.Join());
            return (false);
        }
        config.set_log_nonsensitive(true);
        return (true);
    } else if (cmd == "log_errors") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_errors: " +
                    cmd.Join());
            return (false);
        }
        config.set_log_errors(true);
        return (true);
    } else if (cmd == "log_all") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_all: " + cmd.Join());
            return (false);
        }
        config.set_log_all(true);
        return (true);
    } else {
        // Ignore other commands.
        return(true);
    }
}

bool Load::ConfigCommands(std::vector<Command>& cmds, Config& config) {
    bool result = true;
    for (Command& cmd : cmds) {
        result &= Load::ConfigCommand(cmd, config);
    }
    return (result);
}

