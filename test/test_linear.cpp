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
#include "Gray/VrMath/LinearR3.h"
#include "Gray/Random/Random.h"


TEST(RefVecToMapTest, ReproduceZAxis) {
    const VectorR3 unitz(0, 0, 1);
    for (size_t ii = 0; ii < 100; ii++) {
        VectorR3 axis = Random::UniformSphere();
        auto transform = RefVecToMap(axis);
        EXPECT_LT((transform * unitz - axis).Norm(), 1e-14);
        EXPECT_LT((transform.Inverse() * axis - unitz).Norm(), 1e-14);
    }
}

TEST(RefAxisPlusTransToMapTest, ReproduceZAxisPlusOffset) {
    const VectorR3 unitz(0, 0, 1);
    for (size_t ii = 0; ii < 100; ii++) {
        const VectorR3 axis = Random::UniformSphere();
        const VectorR3 offset = Random::UniformSphere();

        auto transform = RefAxisPlusTransToMap(axis, offset);
        EXPECT_LT((transform * unitz - (axis + offset)).Norm(), 1e-14);
        EXPECT_LT((transform.Inverse() * (axis + offset) - unitz).Norm(), 1e-14);
    }
}
