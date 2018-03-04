/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include  "gtest/gtest.h"
#include "Gray/Random/Random.h"

/*!
 * We use the std::mt19937 implementation of the Mersenne Twister Engine.  Even
 * though this resulting value should be guaranteed by the standard, check to
 * make sure our interface to process the seed doesn't have any problems.
 * http://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
 */
TEST(RandomTest, CorrectVal) {
    Random::SeedDefault();
    unsigned long val;
    for (size_t ii = 0; ii < 10000; ++ii) {
        val = Random::Int();
    }
    EXPECT_EQ(val, 4123659995);
}
