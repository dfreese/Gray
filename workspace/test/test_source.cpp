#include "gtest/gtest.h"
#include <memory>
#include "Sources/VectorSource.h"
#include "Graphics/SceneDescription.h"
#include "Graphics/ViewableSphere.h"

class VectorSourceTest : public ::testing::Test {
public:
    unique_ptr<VectorSource> source;
protected:
    virtual void SetUp() {
        unique_ptr<SceneDescription> scene(new SceneDescription());
        // While we only use triangles in gray with VectorSource, any viewable
        // type is supported.
        scene->AddViewable(new ViewableSphere({0,0,0}, 1));
        source = unique_ptr<VectorSource>(new VectorSource(1.0, std::move(scene)));
    }
};

TEST_F(VectorSourceTest, InsideAtOrigin) {
    EXPECT_TRUE(source->Inside({0, 0, 0}));
}

TEST_F(VectorSourceTest, InsideOnEdge) {
    EXPECT_TRUE(source->Inside({0, 0, -1}));
}

TEST_F(VectorSourceTest, OutsideOnEdge) {
    EXPECT_FALSE(source->Inside({0, 1e-6, -1}));
}

TEST_F(VectorSourceTest, OutsideExtents) {
    EXPECT_FALSE(source->Inside({10, 1e-6, -1}));
}
