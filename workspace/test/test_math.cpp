#include "gtest/gtest.h"
#include "Math/Math.h"
#include "Random/Random.h"

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


