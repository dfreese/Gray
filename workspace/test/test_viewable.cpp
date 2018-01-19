/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "gtest/gtest.h"
#include <cmath>
#include <unordered_map>
#include "VrMath/LinearR3.h"
#include "Gray/LoadDetector.h"

TEST(AnnulusCylinderTest, NoTriangles) {
    auto pieces = LoadDetector::MakeAnnulusCylinder(50, 60, 20);
    ASSERT_EQ(pieces.size(), 8 * 100);
}

TEST(AnnulusCylinderTest, SharedPointsFace) {
    auto pieces = LoadDetector::MakeAnnulusCylinder(50, 60, 20);
    ASSERT_EQ(pieces.size() % 2, 0);
    for (size_t ii = 0; ii < (int) pieces.size(); ii += 2) {
        const auto & first = pieces[ii];
        const auto & second = pieces[ii + 1];
        int no_shared_points = ((first.GetVertexA() == second.GetVertexA()) +
                                (first.GetVertexA() == second.GetVertexB()) +
                                (first.GetVertexA() == second.GetVertexC()) +
                                (first.GetVertexB() == second.GetVertexA()) +
                                (first.GetVertexB() == second.GetVertexB()) +
                                (first.GetVertexB() == second.GetVertexC()) +
                                (first.GetVertexC() == second.GetVertexA()) +
                                (first.GetVertexC() == second.GetVertexB()) +
                                (first.GetVertexC() == second.GetVertexC()));
        EXPECT_EQ(no_shared_points, 2);
    }
}

TEST(AnnulusCylinderTest, SharedPointsAll) {
    const double inner_radius = 50;
    const double outer_radius = 60;
    auto pieces = LoadDetector::MakeAnnulusCylinder(inner_radius,
                                                    outer_radius, 20);
    std::unordered_map<VectorR3, int> point_counts;
    for (const auto & triangle: pieces) {
        point_counts[triangle.GetVertexA()]++;
        point_counts[triangle.GetVertexB()]++;
        point_counts[triangle.GetVertexC()]++;
    }

    for (const auto & point_count: point_counts) {
        const VectorR3 & a = point_count.first;
        const double radius = std::sqrt(a.x * a.x + a.y * a.y);

        constexpr double rel_tol = 1e-6;
        const double tol = inner_radius * rel_tol;
        const bool inner = std::abs(radius - inner_radius) < tol;
        const bool outer = std::abs(radius - outer_radius) < tol;
        EXPECT_TRUE(inner ^ outer);
        // This number is dependent on the rotation model in the implementation
        // but could be 4 or 8 for other implementations.
        EXPECT_EQ(point_count.second, 6);
    }
}

TEST(AnnulusCylinderTest, Radius) {
    const double inner_radius = 50;
    const double outer_radius = 60;
    auto pieces = LoadDetector::MakeAnnulusCylinder(inner_radius,
                                                    outer_radius, 20);
    ASSERT_EQ(pieces.size() % 2, 0);
    for (const auto & triangle: pieces) {
        const VectorR3 & a = triangle.GetVertexA();
        const VectorR3 & b = triangle.GetVertexB();
        const VectorR3 & c = triangle.GetVertexC();
        const double radius_a = std::sqrt(a.x * a.x + a.y * a.y);
        const double radius_b = std::sqrt(b.x * b.x + b.y * b.y);
        const double radius_c = std::sqrt(c.x * c.x + c.y * c.y);

        constexpr double rel_tol = 1e-6;
        const double tol = inner_radius * rel_tol;
        ASSERT_GT(radius_a, inner_radius - tol);
        ASSERT_LT(radius_a, outer_radius + tol);

        const bool a_inner = std::abs(radius_a - inner_radius) < tol;
        const bool b_inner = std::abs(radius_b - inner_radius) < tol;
        const bool c_inner = std::abs(radius_c - inner_radius) < tol;
        const bool a_outer = std::abs(radius_a - outer_radius) < tol;
        const bool b_outer = std::abs(radius_b - outer_radius) < tol;
        const bool c_outer = std::abs(radius_c - outer_radius) < tol;

        const int no_inner = a_inner + b_inner + c_inner;
        const int no_outer = a_outer + b_outer + c_outer;
        const int no_on = no_inner + no_outer;

        ASSERT_EQ(no_on, 3);
    }
}
