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
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include "Gray/Gray/Command.h"
#include "Gray/Gray/Syntax.h"

TEST(CommandTest, BasicSplit) {
    Command cmd("    begin_repeat 5 # ignored comment");
    ASSERT_EQ(cmd.tokens.size(), 2);
    EXPECT_EQ(cmd.tokens[0], "begin_repeat");
    EXPECT_EQ(cmd.tokens[1], "5");
}

TEST(CommandTest, MarkError) {
    std::string err_msg("there are four lights!");
    Command cmd("begin_repeat 5 # ignored comment");
    cmd.MarkError(err_msg);
    EXPECT_EQ(cmd.ErrorMsg(), err_msg);
    EXPECT_TRUE(cmd.IsError());
}

TEST(CommandTest, MarkWarning) {
    std::string warn_msg("there are four lights!");
    Command cmd("begin_repeat 5 # ignored comment");
    cmd.MarkWarning(warn_msg);
    EXPECT_EQ(cmd.WarningMsg(), warn_msg);
    EXPECT_TRUE(cmd.IsWarning());
}

TEST(CommandTest, Parse) {
    Command cmd(" value 1 2 3 0.0 1.0 2.0 # ignored comment");
    int i, j, k;
    double x, y, z;
    EXPECT_TRUE(cmd.parse(i, j, k, x, y, z));
    EXPECT_EQ(i, 1);
    EXPECT_EQ(j, 2);
    EXPECT_EQ(k, 3);
    EXPECT_EQ(x, 0.0);
    EXPECT_EQ(y, 1.0);
    EXPECT_EQ(z, 2.0);

    EXPECT_FALSE(cmd.parse(i, j, k, x, y));
    std::string too_many;
    EXPECT_FALSE(cmd.parse(i, j, k, x, y, z, too_many));
}

TEST(CommandTest, ParseEmpty) {
    Command cmd("   # ignored comment");
    EXPECT_FALSE(cmd.parse());
    EXPECT_TRUE(cmd.parseAll());

    cmd = Command("single_token     # ignored comment");
    EXPECT_TRUE(cmd.parse());

    std::string val;
    EXPECT_TRUE(cmd.parseAll(val));
    EXPECT_EQ(val, "single_token");
}
