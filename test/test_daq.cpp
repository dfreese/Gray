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
#include <memory>
#include "Gray/Daq/Mapping.h"
#include "Gray/Daq/Process.h"
#include "Gray/Daq/ProcessStats.h"
#include "Gray/Daq/ProcessFactory.h"
#include "Gray/Physics/Interaction.h"

TEST(MergeTest, BasicMergeFirst) {
    ProcessFactory::ProcessDescription desc;
    ProcessFactory::ProcessDescLine("merge detector 1.0 first", desc);
    // ProcessFactory::ProcessDescLine("merge detector 1.0 max", desc);
    Mapping::IdMappingT mapping = {{"detector", {0}}};

    auto proc = ProcessFactory::ProcessFactory(desc, mapping);

    std::vector<Interaction> events(4);
    std::vector<Process::TimeT> times({0.0, 0.99, 3.0, 4.0});
    std::vector<float> energy({1.0, 2.0, 3.0, 4.0});

    for (size_t ii = 0; ii < events.size(); ++ii) {
        events[ii].time = times[ii];
        events[ii].energy = energy[ii];
    }

    ProcessStats stats;
    auto ready = proc->process(events.begin(), events.end(), stats);
    EXPECT_EQ(ready, events.end() - 1);

    proc->stop(ready, events.end(), stats);
    EXPECT_EQ(stats.no_dropped, 1);
    EXPECT_EQ(stats.no_kept, 3);
    EXPECT_EQ(events[1].dropped, true);
    EXPECT_EQ(events[0].energy, energy[0] + energy[1]);
}

TEST(MergeTest, BasicMergeMax) {
    ProcessFactory::ProcessDescription desc;
    ProcessFactory::ProcessDescLine("merge detector 1.0 max", desc);
    Mapping::IdMappingT mapping = {{"detector", {0}}};

    auto proc = ProcessFactory::ProcessFactory(desc, mapping);

    std::vector<Interaction> events(4);
    std::vector<Process::TimeT> times({0.0, 0.99, 3.0, 4.0});
    std::vector<float> energy({1.0, 2.0, 3.0, 4.0});

    for (size_t ii = 0; ii < events.size(); ++ii) {
        events[ii].time = times[ii];
        events[ii].energy = energy[ii];
    }

    ProcessStats stats;
    auto ready = proc->process(events.begin(), events.end(), stats);
    EXPECT_EQ(ready, events.end() - 1);

    proc->stop(ready, events.end(), stats);
    EXPECT_EQ(stats.no_dropped, 1);
    EXPECT_EQ(stats.no_kept, 3);
    EXPECT_EQ(events[0].dropped, true);
    EXPECT_EQ(events[1].energy, energy[0] + energy[1]);
}
