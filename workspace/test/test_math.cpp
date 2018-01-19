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
#include <algorithm>
#include <numeric>
#include "Math/Math.h"

class InterpolateTest : public ::testing::Test {
public:
    const std::vector<double> x = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7};
    const std::vector<double> y = {0.0, 0.5, -0.2, 1.3, 18.0, 50.0, -30, 0.7};
};

TEST_F(InterpolateTest, Below) {
    EXPECT_DOUBLE_EQ(Math::interpolate(x, y, -1.0), y.front());
}

TEST_F(InterpolateTest, Above) {
    EXPECT_DOUBLE_EQ(Math::interpolate(x, y, 1.0), y.back());
}

TEST_F(InterpolateTest, On) {
    const size_t idx = 4;
    EXPECT_DOUBLE_EQ(Math::interpolate(x, y, x[idx]), y[idx]);
}

TEST_F(InterpolateTest, Between) {
    const size_t idx = 4;
    const double alpha = 0.75;
    const double x_val = x[idx] + alpha * (x[idx + 1] - x[idx]);
    const double y_val_exp = (1 - alpha) * y[idx] + alpha * y[idx + 1];
    EXPECT_DOUBLE_EQ(Math::interpolate(x, y, x_val), y_val_exp);
}


class Interpolate2dTest : public ::testing::Test {
public:
    const std::vector<double> x = {0.0, 0.1, 0.2, 0.3,};
    const std::vector<double> y = {0.0, 0.1, 0.2, 0.3,};
    const std::vector<std::vector<double>> z = {
        {0.0, 0.5, -0.2, 1.3,},
        {18.0, 50.0, -30, 0.7},
        {10.0, 12.0, 13.0, 14.0},
        {8.0, 10, 11, 0.7},
    };
};

TEST_F(Interpolate2dTest, BelowBoth) {
    EXPECT_DOUBLE_EQ(Math::interpolate_2d(x, y, z, -1.0, -1.0),
                     z.front().front());
}

TEST_F(Interpolate2dTest, AboveBoth) {
    EXPECT_DOUBLE_EQ(Math::interpolate_2d(x, y, z, 1.0, 1.0),
                     z.back().back());
}

TEST_F(Interpolate2dTest, BelowX) {
    EXPECT_DOUBLE_EQ(Math::interpolate_2d(x, y, z, -1.0, (y[0] + y[1]) / 2),
                     (z[0][0] + z[0][1]) / 2);
}

TEST_F(Interpolate2dTest, BelowY) {
    EXPECT_DOUBLE_EQ(Math::interpolate_2d(x, y, z, (x[0] + x[1]) / 2, -1.0),
                     (z[0][0] + z[1][0]) / 2);
}

TEST_F(Interpolate2dTest, Between) {
    const double exp_z = (z[0][0] + z[0][1] + z[1][0] + z[1][1]) / 4;
    EXPECT_DOUBLE_EQ(exp_z, Math::interpolate_2d(x, y, z,
                                                 (x[0] + x[1]) / 2,
                                                 (y[0] + y[1]) / 2));
}


class InterpolateY2dTest : public ::testing::Test {
public:
    const std::vector<double> x = {0.0, 0.1, 0.2, 0.3,};
    const std::vector<double> y = {0.0, 0.1, 0.2, 0.3,};
    const std::vector<std::vector<double>> z = {
        {0.0, 0.1, 0.6, 1.0,},
        {0.0, 0.1, 0.7, 1.0,},
        {0.0, 0.2, 0.8, 1.0,},
        {0.0, 0.4, 0.9, 1.0,},
    };
};

TEST_F(InterpolateY2dTest, BelowZ) {
    for (double x_val: {-1.0, 0.15, 0.25, 4.0}) {
        EXPECT_DOUBLE_EQ(Math::interpolate_y_2d(x, y, z, x_val, -1.0), y[0]);
    }
}

TEST_F(InterpolateY2dTest, OnXandZ) {
    EXPECT_DOUBLE_EQ(Math::interpolate_y_2d(x, y, z, 0.1, 0.7), y[2]);
}

TEST_F(InterpolateY2dTest, OnX) {
    EXPECT_DOUBLE_EQ(Math::interpolate_y_2d(x, y, z, 0.1, 0.4),
                     (y[1] + y[2]) / 2);
}

class LinspaceTest : public ::testing::Test {
public:
    const double start = 30.0;
    const double end = 60.0;
    const int no_points = 31;
    const std::vector<double> x = Math::linspace(start, end, no_points);
};

TEST_F(LinspaceTest, Start) {
    EXPECT_EQ(x.front(), start);
}

TEST_F(LinspaceTest, End) {
    EXPECT_EQ(x.back(), end);
}

TEST_F(LinspaceTest, Spacing) {
    std::vector<double> diff(no_points);
    std::adjacent_difference(x.begin(), x.end(), diff.begin());
    EXPECT_TRUE(std::all_of(diff.begin() + 1, diff.end(), [](double val) {
        return (val == 1);
    }));
}

TEST(TrapZTest, Uniform) {
    const double start = 0;
    const double end = 10.0;
    const std::vector<double> x = Math::linspace(start, end, 100);
    const std::vector<double> y(x.size(), 1);

    auto result = Math::trap_z(x, y);
    EXPECT_LT((end - result.back()) / end, 1e-12);
}

TEST(TrapZTest, NonUniform) {
    const double start = 0;
    const double end = 10.0;
    const std::vector<double> x1 = Math::linspace(start, end / 2, 100);
    const std::vector<double> x2 = Math::linspace(end / 2 + 1e-3, end, 20);
    std::vector<double> x(x1);
    x.insert(x.end(), x2.begin(), x2.end());
    const std::vector<double> y(x.size(), 1);

    auto result = Math::trap_z(x, y);
    EXPECT_LT((end - result.back()) / end, 1e-12);
}

TEST(TrapZTest, AverageZero) {
    const double start = 0;
    const double end = 10.0;
    const std::vector<double> x = Math::linspace(start, end, 100);
    std::vector<double> y(x.size(), 1);
    for (size_t ii = 1; ii < y.size(); ii += 2) {
        y[ii] = -1;
    }

    auto result = Math::trap_z(x, y);
    EXPECT_EQ(result.back(), 0);
}

TEST(PdfToCdfTest, Uniform) {
    const double start = 0;
    const double end = 10.0;
    const std::vector<double> x = Math::linspace(start, end, 100);
    const std::vector<double> y(x.size(), 1);

    auto result = Math::pdf_to_cdf(x, y);
    EXPECT_DOUBLE_EQ(0.0, result.front());
    EXPECT_DOUBLE_EQ(1.0, result.back());
}

TEST(PdfToCdfTest, Scaling) {
    const double start = 0;
    const double end = 10.0;
    const std::vector<double> x = Math::linspace(start, end, 100);
    const std::vector<double> y1(x.size(), 1);
    const std::vector<double> y2(x.size(), 2);

    auto result1 = Math::pdf_to_cdf(x, y1);
    auto result2 = Math::pdf_to_cdf(x, y2);
    EXPECT_TRUE(std::equal(result1.begin(), result1.end(), result2.begin()));
}

class InterpolateEdgeTest : public ::testing::Test {
public:
    const std::vector<double> x = {0.0, 0.1, 0.2, 0.3, 0.3, 0.5, 0.6, 0.7};
    const std::vector<double> y = {10.0, 9.0, 8.0, 7.0, 11.0, 10.0, 9.0, 8.0};
};

TEST_F(InterpolateEdgeTest, Edge) {
    EXPECT_NEAR(Math::interpolate(x, y, 0.299), y[3], 1e-2);
    EXPECT_NEAR(Math::interpolate(x, y, 0.3), y[4], 1e-2);
}
