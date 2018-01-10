#include "gtest/gtest.h"
#include <limits>
#include <memory>
#include "Physics/Positron.h"
#include "Sources/SphereSource.h"
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
        std::unique_ptr<ViewableSphere> sphere(new ViewableSphere({0,0,0}, 1));
        scene->AddViewable(std::move(sphere));
        source = unique_ptr<VectorSource>(new VectorSource(1.0, std::move(scene)));
    }
};

TEST_F(VectorSourceTest, InsideAtOrigin) {
    EXPECT_TRUE(source->Inside({0, 0, 0}));
}

TEST_F(VectorSourceTest, InsideOnEdge) {
    EXPECT_TRUE(source->Inside({0, 0, -1 + 1e-6}));
}

TEST_F(VectorSourceTest, OutsideOnEdge) {
    EXPECT_FALSE(source->Inside({0, 1e-6, -1}));
}

TEST_F(VectorSourceTest, OutsideExtents) {
    EXPECT_FALSE(source->Inside({10, 1e-6, -1}));
}

TEST(Source, HalfLife) {
    const double act = 1.0;
    const double act_uCi = act / Physics::decays_per_microcurie;
    SphereSource sp({0, 0, 0}, 1.0, act_uCi);
    Positron pos;

    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 1.0, 0);
    sp.SetIsotope(std::make_shared<Positron>(pos));
    EXPECT_EQ(sp.GetActivity(0.0), act);
    EXPECT_EQ(sp.GetActivity(1.0), act);
    EXPECT_EQ(sp.GetActivity(std::numeric_limits<double>::max()), act);


    double half_life = 1.0;
    pos = Positron(0.0, 1.0, half_life, 0);
    sp.SetIsotope(std::make_shared<Positron>(pos));
    EXPECT_EQ(sp.GetActivity(0.0 * half_life), act);
    EXPECT_EQ(sp.GetActivity(1.0 * half_life), act / 2.0);
    EXPECT_EQ(sp.GetActivity(2.0 * half_life), act / 4.0);
    EXPECT_EQ(sp.GetActivity(std::numeric_limits<double>::max()), 0.0);
}
