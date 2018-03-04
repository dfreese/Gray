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
#include "Gray/Physics/Positron.h"
#include "Gray/Sources/VectorSource.h"
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Graphics/ViewableSphere.h"

TEST(Isotope, FractionRemaining) {
    Positron pos;
    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 1.0, 0);
    EXPECT_EQ(pos.FractionRemaining(0.0), 1.0);
    EXPECT_EQ(pos.FractionRemaining(1.0), 1.0);
    EXPECT_EQ(pos.FractionRemaining(std::numeric_limits<double>::max()), 1.0);

    double half_life = 1.0;
    pos = Positron(0.0, 1.0, half_life, 0);
    EXPECT_EQ(pos.FractionRemaining(0.0), 1.0);
    EXPECT_EQ(pos.FractionRemaining(1.0 * half_life), 0.5);
    EXPECT_EQ(pos.FractionRemaining(2.0 * half_life), 0.25);
    EXPECT_EQ(pos.FractionRemaining(std::numeric_limits<double>::max()), 0.0);
}

TEST(Isotope, FractionIntegral) {
    Positron pos;
    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 1.0, 0);
    EXPECT_EQ(pos.FractionIntegral(0.0, 0.0), 0.0);
    EXPECT_EQ(pos.FractionIntegral(0.0, 1.0), 1.0);

    double half_life = 1.0;
    pos = Positron(0.0, 1.0, half_life, 0);
    EXPECT_EQ(pos.FractionIntegral(0.0, 0.0), 0.0);
    EXPECT_NEAR(pos.FractionIntegral(0.0, 1.0), 0.721348, 1e-6);
    EXPECT_NEAR(pos.FractionIntegral(1.0, 1.0), 0.360674, 1e-6);
}


TEST(Positron, ExpectedPhotons) {
    Positron pos;
    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 1.0, 0);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 2.0);

    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 0.5, 0);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 1.0);

    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 0.25, 1);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 1.5);
}
