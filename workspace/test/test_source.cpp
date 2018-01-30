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
#include <limits>
#include <memory>
#include "Physics/Positron.h"
#include "Sources/SourceList.h"
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

TEST(Source, ExpectedDecaysAndPhotons) {
    const double act = 1.0;
    const double act_uCi = act / Physics::decays_per_microcurie;
    SphereSource sp({0, 0, 0}, 1.0, act_uCi);
    Positron pos;

    pos = Positron(0.0, std::numeric_limits<double>::infinity(), 1.0, 0);
    double phots = pos.ExpectedNoPhotons();
    sp.SetIsotope(std::make_shared<Positron>(pos));
    EXPECT_EQ(sp.GetExpectedDecays(0.0, 0.0), 0.0);
    EXPECT_EQ(sp.GetExpectedDecays(0.0, 1.0), 1.0 * act);
    EXPECT_EQ(sp.GetExpectedDecays(0.0, 2.0), 2.0 * act);
    EXPECT_EQ(sp.GetExpectedPhotons(0.0, 1.0), 1.0 * act * phots);
    EXPECT_EQ(sp.GetExpectedPhotons(0.0, 2.0), 2.0 * act * phots);

    double half_life = 1.0;
    pos = Positron(0.0, 1.0, half_life, 0);
    phots = pos.ExpectedNoPhotons();
    sp.SetIsotope(std::make_shared<Positron>(pos));
    EXPECT_NEAR(sp.GetExpectedDecays(0.0, 1.0), 0.721348 * act, 1e-6);
    EXPECT_NEAR(sp.GetExpectedDecays(1.0, 1.0), 0.360674 * act, 1e-6);
    EXPECT_NEAR(sp.GetExpectedPhotons(0.0, 1.0), 0.721348 * act * phots, 1e-6);
    EXPECT_NEAR(sp.GetExpectedPhotons(1.0, 1.0), 0.360674 * act * phots, 1e-6);
}

TEST(SourceList, SearchSplitTime) {
    double half_life = 1.0;
    const double act = 1.0;
    const double act_uCi = act / Physics::decays_per_microcurie;
    SourceList list;
    list.AddIsotope("test",
            std::unique_ptr<Isotope>(new Positron(0.0, 1.0, half_life, 0)));
    list.SetCurIsotope("test", RigidMapR3());

    list.AddSource(std::unique_ptr<Source>(
                new SphereSource({0, 0, 0}, 1.0, act_uCi)));
    list.SetSimulationTime(2.0);

    double exp_phot = list.ExpectedPhotons(0.0, 2.0);
    EXPECT_NEAR(exp_phot, 1.0820212 * act * 2.0, 1e-6);

    double time = list.SearchSplitTime(0, 2.0, 0, exp_phot);
    EXPECT_EQ(time, 2.0);

    time = list.SearchSplitTime(0, 2.0, 0, exp_phot / 2.0);
    EXPECT_NEAR(list.ExpectedPhotons(0, time), exp_phot / 2.0, 1e-5);
    EXPECT_NEAR(list.ExpectedPhotons(time, 2.0 - time), exp_phot / 2.0, 1e-5);
}
