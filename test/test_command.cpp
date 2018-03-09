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

