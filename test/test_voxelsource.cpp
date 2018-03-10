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
#include <array>
#include <sstream>
#include <vector>
#include "Gray/Sources/VoxelSource.h"

TEST(VoxelSourceTest, LoadEasy) {
    std::stringstream ss;
    int magic_number = 65531;
    int version_number = 1;
    std::array<int,3> exp_dims{{1, 1, 1}};
    float val = 42.0;
    ss.write(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
    ss.write(reinterpret_cast<char*>(&version_number), sizeof(version_number));
    ss.write(reinterpret_cast<char*>(exp_dims.data()),
            exp_dims.size() * sizeof(int));
    ss.write(reinterpret_cast<char*>(&val), sizeof(val));

    std::array<int,3> dims;
    std::vector<double> vals;
    ASSERT_TRUE(VoxelSource::Load(ss, vals, dims));
    EXPECT_EQ(dims, exp_dims);
    ASSERT_EQ(vals.size(), 1);
    EXPECT_EQ(vals.front(), 42.0);
}

TEST(VoxelSourceTest, LoadXZYOrder) {
    std::stringstream ss;
    int magic_number = 65531;
    int version_number = 1;
    std::array<int,3> exp_dims{{2, 2, 2}};
    // In XZY ordering
    std::vector<float> write_vals({0, 2, 1, 3, 4, 6, 5, 7});
    ss.write(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
    ss.write(reinterpret_cast<char*>(&version_number), sizeof(version_number));
    ss.write(reinterpret_cast<char*>(exp_dims.data()),
            exp_dims.size() * sizeof(int));
    ss.write(reinterpret_cast<char*>(write_vals.data()),
            write_vals.size() * sizeof(float));

    // XYZ ordering
    std::vector<double> exp_vals = {0, 1, 2, 3, 4, 5, 6, 7};

    std::array<int,3> dims;
    std::vector<double> vals;
    ASSERT_TRUE(VoxelSource::Load(ss, vals, dims));
    EXPECT_EQ(dims, exp_dims);
    EXPECT_EQ(vals, exp_vals);
}

TEST(VoxelSourceTest, LoadBadMagic) {
    std::stringstream ss;
    int magic_number = 65532;
    ss.write(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));

    std::array<int,3> dims;
    std::vector<double> vals;
    ASSERT_FALSE(VoxelSource::Load(ss, vals, dims));
}

TEST(VoxelSourceTest, LoadBadVersion) {
    std::stringstream ss;
    int magic_number = 65532;
    int version_number = 2;
    ss.write(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
    ss.write(reinterpret_cast<char*>(&version_number), sizeof(version_number));

    std::array<int,3> dims;
    std::vector<double> vals;
    ASSERT_FALSE(VoxelSource::Load(ss, vals, dims));
}
