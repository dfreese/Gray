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
#include "Gray/Gray/File.h"

TEST(FileTest, DirNone) {
    EXPECT_EQ(File::DirWindows("test_name.dat"), "");
    EXPECT_EQ(File::DirPosix("test_name.dat"), "");
    // Should work on any platform
    EXPECT_EQ(File::Dir("test_name.dat"), "");
}

TEST(FileTest, DirAbsolute) {
    EXPECT_EQ(
            File::DirPosix("/usr/opt/local/test/test_name.dat"),
            "/usr/opt/local/test");
    EXPECT_EQ(
            File::DirWindows("C:\\Users\\Gray\\Documents\\test_name.dat"),
            "C:\\Users\\Gray\\Documents");
}

TEST(FileTest, DirRelative) {
    EXPECT_EQ(File::DirPosix("../test/test_name.dat"), "../test");
    EXPECT_EQ(
            File::DirWindows("..\\Documents\\test_name.dat"),
            "..\\Documents");
}

TEST(FileTest, JoinEmptyDir) {
    EXPECT_EQ(File::JoinWindows("", "test_name.dat"), "test_name.dat");
    EXPECT_EQ(File::JoinPosix("", "test_name.dat"), "test_name.dat");
    // Should work on any platform
    EXPECT_EQ(File::Join("", "test_name.dat"), "test_name.dat");
}

TEST(FileTest, JoinDirWithSlash) {
    EXPECT_EQ(
            File::JoinWindows("C:\\Users\\Gray\\Documents\\", "test_name.dat"),
            "C:\\Users\\Gray\\Documents\\test_name.dat");
    EXPECT_EQ(
            File::JoinPosix("/usr/opt/local/test/", "test_name.dat"),
            "/usr/opt/local/test/test_name.dat");
}

TEST(FileTest, JoinDirWithoutSlash) {
    EXPECT_EQ(
            File::JoinWindows("C:\\Users\\Gray\\Documents", "test_name.dat"),
            "C:\\Users\\Gray\\Documents\\test_name.dat");
    EXPECT_EQ(
            File::JoinPosix("/usr/opt/local/test", "test_name.dat"),
            "/usr/opt/local/test/test_name.dat");
}
