#include "gtest/gtest.h"
#include <memory>
#include "Physics/Positron.h"
#include "Sources/VectorSource.h"
#include "Graphics/SceneDescription.h"
#include "Graphics/ViewableSphere.h"

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

TEST(Positron, ExpectedPhotons) {
    Positron pos;
    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 1.0, 0);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 2.0);

    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 0.5, 0);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 1.0);

    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 0.25, 1);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 1.5);
}
