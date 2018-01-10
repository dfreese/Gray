#include "gtest/gtest.h"
#include <memory>
#include "Physics/Positron.h"
#include "Sources/VectorSource.h"
#include "Graphics/SceneDescription.h"
#include "Graphics/ViewableSphere.h"

TEST(Positron, ExpectedPhotons) {
    Positron pos;
    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 1.0, 0);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 2.0);

    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 0.5, 0);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 1.0);

    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 0.25, 1);
    EXPECT_EQ(pos.ExpectedNoPhotons(), 1.5);
}
