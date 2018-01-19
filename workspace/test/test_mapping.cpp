/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include  "gtest/gtest.h"
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include "Daq/Process.h"
#include "Daq/Mapping.h"

TEST(LoadMappingTest, Basic) {
    using DetIdT = Mapping::DetIdT;
    std::stringstream ss("detector block bx by bz\n"
                         "0 0 0 0 0\n"
                         "1 0 1 0 0\n"
                         "2 0 0 1 0\n"
                         "3 0 1 1 0 # has a comment\n"
                         "4 0 0 0 1\n"
                         "   5 0 1 0 1 # has leading whitespace too\n"
                         "6 0 0 1 1\n"
                         "7 0 1 1 1\n");
    const std::vector<DetIdT> det_exp = {0, 1, 2, 3, 4, 5, 6, 7};
    const std::vector<DetIdT> blk_exp = {0, 0, 0, 0, 0, 0, 0, 0};
    const std::vector<DetIdT> bx_exp = {0, 1, 0, 1, 0, 1, 0, 1};
    const std::vector<DetIdT> by_exp = {0, 0, 1, 1, 0, 0, 1, 1};
    const std::vector<DetIdT> bz_exp = {0, 0, 0, 0, 1, 1, 1, 1};
    const std::map<std::string, std::vector<DetIdT>> exp_mapping = {
        {"detector", det_exp},
        {"block", blk_exp},
        {"bx", bx_exp},
        {"by", by_exp},
        {"bz", bz_exp},
    };
    std::map<std::string, std::vector<DetIdT>> mapping;
    const int no_detectors = static_cast<int>(det_exp.size());
    int status = Mapping::LoadMapping(ss, mapping);
    ASSERT_EQ(status, no_detectors);
    EXPECT_EQ(exp_mapping, mapping);
}

TEST(WriteMappingTest, Roundtrip) {
    std::stringstream ss("detector block bx by bz\n"
                         "0 0 0 0 0\n"
                         "1 0 1 0 0\n"
                         "2 0 0 1 0\n"
                         "3 0 1 1 0 # has a comment\n"
                         "4 0 0 0 1\n"
                         "   5 0 1 0 1 # has leading whitespace too\n"
                         "6 0 0 1 1\n"
                         "7 0 1 1 1\n");

    // The output column order will be in ascending order of the strings for
    // the mappings.  This could change if the underlying container changes
    // from a map.
    std::string exp("block bx by bz detector\n"
                    "0 0 0 0 0\n"
                    "0 1 0 0 1\n"
                    "0 0 1 0 2\n"
                    "0 1 1 0 3\n"
                    "0 0 0 1 4\n"
                    "0 1 0 1 5\n"
                    "0 0 1 1 6\n"
                    "0 1 1 1 7\n");
    Mapping::IdMappingT mapping;
    Mapping::LoadMapping(ss, mapping);

    std::stringstream ss_output;
    Mapping::WriteMapping(ss_output, mapping);
    EXPECT_EQ(exp, ss_output.str());

    // Complete the roundtrip and make sure we read in the same thing.
    Mapping::IdMappingT exp_mapping;
    Mapping::LoadMapping(ss_output, exp_mapping);
    EXPECT_EQ(exp_mapping, mapping);
}
