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
#include <vector>
#include "Gray/Gray/String.h"

TEST(SplitTest, EmptyString) {
    std::vector<std::string> val = String::Split("", " ");
    std::vector<std::string> exp;
    EXPECT_EQ(val, exp);
}

TEST(SplitTest, Basic) {
    std::vector<std::string> val = String::Split("hello world", " ");
    std::vector<std::string> exp({"hello", "world"});
    EXPECT_EQ(val, exp);
}

TEST(SplitTest, EmptySplit) {
    std::vector<std::string> val = String::Split("hello  world", " ");
    std::vector<std::string> exp({"hello", "world"});
    EXPECT_EQ(val, exp);
}

TEST(SplitTest, MultipleDelim) {
    std::vector<std::string> val = String::Split("hello, world", " ,");
    std::vector<std::string> exp({"hello", "world"});
    EXPECT_EQ(val, exp);
}

TEST(SplitTest, ManyVal) {
    std::vector<std::string> val = String::Split("  it was the best of times  ", " ,");
    std::vector<std::string> exp({"it", "was", "the", "best", "of", "times"});
    EXPECT_EQ(val, exp);
}

TEST(StringTest, Parse) {
    std::vector<std::string> vals({"1.0", "3", "hello"});
    double a;
    int b;
    std::string c;
    EXPECT_TRUE(String::Parse(vals.begin(), vals.end(), a, b, c));
    EXPECT_EQ(a, 1.0);
    EXPECT_EQ(b, 3);
    EXPECT_EQ(c, "hello");

    float d;
    // float can't hold a string
    EXPECT_FALSE(String::Parse(vals.begin(), vals.end(), a, b, d));
    // too few parameters
    EXPECT_FALSE(String::Parse(vals.begin(), vals.end(), a, b));
    // too few strings
    EXPECT_FALSE(String::Parse(vals.begin(), vals.end(), a, b, c, d));
}
