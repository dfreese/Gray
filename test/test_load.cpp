/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "gtest/gtest.h"
#include <string>
#include "Gray/Gray/Config.h"
#include "Gray/Gray/Load.h"
#include "Gray/Gray/Syntax.h"
#include "Gray/Output/Output.h"

TEST(LoadTest, HitsFormat) {
    Config config;
    Command cmd("hits_format var_binary");

    // var_ascii is default
    EXPECT_EQ(config.get_format_hits(), Output::Format::VariableAscii);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_format_hits(), Output::Format::VariableBinary);

    cmd = Command("hits_format var_ascii");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("hits_format var_ascii garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("hits_format ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, SinglesFormat) {
    Config config;
    Command cmd("singles_format var_binary");

    // var_ascii is default
    EXPECT_EQ(config.get_format_singles(), Output::Format::VariableAscii);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_format_singles(), Output::Format::VariableBinary);

    cmd = Command("singles_format var_ascii");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("singles_format var_ascii garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("singles_format ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, CoincFormat) {
    Config config;
    Command cmd("coinc_format var_binary");

    // var_ascii is default
    EXPECT_EQ(config.get_format_coinc(), Output::Format::VariableAscii);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_format_coinc(), Output::Format::VariableBinary);

    cmd = Command("coinc_format var_ascii");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("coinc_format var_ascii garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("coinc_format ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, HitsOutput) {
    Config config;
    Command cmd("hits_output test.dat");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_filename_hits(), "test.dat");

    cmd = Command("hits_output test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("hits_output ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, SinglesOutput) {
    Config config;
    Command cmd("singles_output test.dat");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_filename_singles(), "test.dat");

    cmd = Command("singles_output test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("singles_output ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, CoincOutput) {
    Config config;

    Command cmd("coinc_output test.dat");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("coinc_output check.dat");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    std::vector<std::string> exp({"test.dat", "check.dat"});
    EXPECT_EQ(config.get_filenames_coinc(), exp);


    cmd = Command("coinc_output test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("coinc_output ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, ProcessFile) {
    Config config;
    Command cmd("process_file test.dat");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_filename_process(), "test.dat");

    cmd = Command("process_file test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("process_file ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, ProcessLine) {
    Config config;
    Command cmd("process merge detector 10e-9 max");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("process merge block 10e-9 anger bx by bz");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    std::vector<std::string> exp({
            "merge detector 10e-9 max",
            "merge block 10e-9 anger bx by bz"});
    EXPECT_EQ(config.get_process_lines(), exp);

    cmd = Command("process ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, MappingFile) {
    Config config;
    Command cmd("mapping_file test.dat");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_filename_mapping(), "test.dat");

    cmd = Command("mapping_file test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("mapping_file ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, HitsVarMask) {
    Config config;
    Command cmd("hits_var_mask 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    // all fields are on by default
    EXPECT_TRUE(config.get_hits_var_output_write_flags().time);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_FALSE(config.get_hits_var_output_write_flags().time);

    cmd = Command("hits_var_mask 0 a 0 0 0 0 0 0 0 0 0 0 0 0 0");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, SinglesVarMask) {
    Config config;
    Command cmd("singles_var_mask 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    // all fields are on by default
    EXPECT_TRUE(config.get_singles_var_output_write_flags().time);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_FALSE(config.get_singles_var_output_write_flags().time);

    cmd = Command("singles_var_mask 0 a 0 0 0 0 0 0 0 0 0 0 0 0 0");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, CoincVarMask) {
    Config config;
    Command cmd("coinc_var_mask 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    // all fields are on by default
    EXPECT_TRUE(config.get_coinc_var_output_write_flags().time);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_FALSE(config.get_coinc_var_output_write_flags().time);

    cmd = Command("coinc_var_mask 0 a 0 0 0 0 0 0 0 0 0 0 0 0 0");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, Time) {
    Config config;
    Command cmd("time 100.0");

    EXPECT_EQ(config.get_time(), 0.0);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_time(), 100.0);

    cmd = Command("time 100.0 help");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("time ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, StartTime) {
    Config config;
    Command cmd("start_time 100.0");

    EXPECT_EQ(config.get_start_time(), 0.0);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_start_time(), 100.0);

    cmd = Command("start_time 100.0 help");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("start_time ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, Seed) {
    Config config;
    Command cmd("seed 100");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_seed(), 100);

    cmd = Command("seed 100 garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("seed ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_positron) {
    Config config;
    Command cmd("log_positron");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_nuclear_decays());

    cmd = Command("log_positron garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_nondepositing) {
    Config config;
    Command cmd("log_nondepositing");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_nondepositing_inter());

    cmd = Command("log_nondepositing garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_nonsensitive) {
    Config config;
    Command cmd("log_nonsensitive");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_nonsensitive());

    cmd = Command("log_nonsensitive garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_errors) {
    Config config;
    Command cmd("log_errors");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_errors());

    cmd = Command("log_errors garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_all) {
    Config config;
    Command cmd("log_all");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_all());

    cmd = Command("log_all garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, ConfigCommands) {
    Config config;
    std::vector<Command> cmds;
    cmds.emplace_back("seed 100");
    cmds.emplace_back("start_time 5.0");
    cmds.emplace_back("time 25.0");
    cmds.emplace_back("log_all");
    EXPECT_TRUE(Load::ConfigCommands(cmds, config));

    EXPECT_EQ(config.get_seed(), 100);
    EXPECT_EQ(config.get_time(), 25.0);
    EXPECT_EQ(config.get_start_time(), 5.0);
    EXPECT_TRUE(config.get_log_all());
}

TEST(LoadTest, ConfigCommandsFailure) {
    Config config;
    std::vector<Command> cmds;
    cmds.emplace_back("seed 100");
    cmds.emplace_back("start_time 5.0");
    cmds.emplace_back("time 25.0 garbage");
    cmds.emplace_back("log_all");
    EXPECT_FALSE(Load::ConfigCommands(cmds, config));

    EXPECT_EQ(config.get_seed(), 100);
    EXPECT_EQ(config.get_time(), 0.0);
    EXPECT_EQ(config.get_start_time(), 5.0);
    EXPECT_TRUE(config.get_log_all());
}

TEST(LoadTest, ConfigCommandsUnknown) {
    Config config;
    std::vector<Command> cmds;
    cmds.emplace_back("seed 100");
    cmds.emplace_back("start_time 5.0");
    cmds.emplace_back("unknown_time 25.0");
    cmds.emplace_back("log_all");
    EXPECT_TRUE(Load::ConfigCommands(cmds, config));

    EXPECT_EQ(config.get_seed(), 100);
    EXPECT_EQ(config.get_time(), 0.0);
    EXPECT_EQ(config.get_start_time(), 5.0);
    EXPECT_TRUE(config.get_log_all());
}
