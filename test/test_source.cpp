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
#include "Gray/Physics/Beam.h"
#include "Gray/Physics/GaussianBeam.h"
#include "Gray/Physics/Positron.h"
#include "Gray/Sources/SourceList.h"
#include "Gray/Sources/SphereSource.h"
#include "Gray/Sources/VectorSource.h"
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Graphics/ViewableSphere.h"

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

TEST(SourceList, IsotopeFactoryNone) {
    Json::Value iso_json;
    Json::Reader reader;

    // Use raw string literal to avoid escaping every quote.
    // http://en.cppreference.com/w/cpp/language/string_literal
    reader.parse(R"isotope_json({
            "acolinearity_deg_fwhm": 0.0,
            "default": true,
            "half_life_s": 0.0,
            "model": "none",
            "positron_emiss_prob": 1.0,
            "prompt_gamma_energy_mev": 0.0
        })isotope_json",
        iso_json, /*collect_comments=*/false);
    auto isotope = SourceList::IsotopeFactory(iso_json, true);
    ASSERT_TRUE(isotope);

    Positron exp(0.0, std::numeric_limits<double>::infinity(), 1.0, 0.0);
    Positron val(dynamic_cast<Positron&>(*isotope));
    EXPECT_EQ(val, exp);
}

TEST(SourceList, IsotopeFactoryGauss) {
    Json::Value iso_json;
    Json::Reader reader;
    // Use raw string literal to avoid escaping every quote.
    // http://en.cppreference.com/w/cpp/language/string_literal
    reader.parse(R"isotope_json({
            "acolinearity_deg_fwhm": 0.0,
            "default": true,
            "half_life_s": 0.0,
            "model": "levin_exp",
            "k1": 24.5,
            "k2": 1.76,
            "model": "gauss",
            "fwhm_mm": 1.5,
            "prob_c": 0.501,
            "max_range_mm": 3.0,
            "positron_emiss_prob": 1.0,
            "prompt_gamma_energy_mev": 0.0
        })isotope_json",
        iso_json, /*collect_comments=*/false);
    auto isotope = SourceList::IsotopeFactory(iso_json, true);
    ASSERT_TRUE(isotope);

    Positron exp(0.0, std::numeric_limits<double>::infinity(), 1.0, 0.0);
    exp.SetPositronRange(1.5, 3.0);
    Positron val(dynamic_cast<Positron&>(*isotope));
    EXPECT_EQ(val, exp);
}

TEST(SourceList, IsotopeFactoryDblExp) {
    Json::Value iso_json;
    Json::Reader reader;
    // Use raw string literal to avoid escaping every quote.
    // http://en.cppreference.com/w/cpp/language/string_literal
    reader.parse(R"isotope_json({
            "acolinearity_deg_fwhm": 0.0,
            "default": true,
            "half_life_s": 0.0,
            "model": "levin_exp",
            "k1": 24.5,
            "k2": 1.76,
            "prob_c": 0.501,
            "max_range_mm": 3.0,
            "positron_emiss_prob": 1.0,
            "prompt_gamma_energy_mev": 0.0
        })isotope_json",
        iso_json, /*collect_comments=*/false);
    auto isotope = SourceList::IsotopeFactory(iso_json, true);
    ASSERT_TRUE(isotope);

    Positron exp(0.0, std::numeric_limits<double>::infinity(), 1.0, 0.0);
    exp.SetPositronRange(0.501, 24.5, 1.76, 3.0);
    Positron val(dynamic_cast<Positron&>(*isotope));
    EXPECT_EQ(val, exp);
}

TEST(SourceList, LoadIsotopes) {
    auto phys_json = R"json({
    "isotopes": {
        "BackBack": {
            "acolinearity_deg_fwhm": 0.0,
            "default": true,
            "half_life_s": 0.0,
            "model": "none",
            "positron_emiss_prob": 1.0,
            "prompt_gamma_energy_mev": 0.0
        },
        "C11": {
            "acolinearity_deg_fwhm": 0.57,
            "half_life_s": 1221.66,
            "k1": 24.5,
            "k2": 1.76,
            "max_range_mm": 3.0,
            "model": "levin_exp",
            "positron_emiss_prob": 0.9975,
            "prob_c": 0.501,
            "prompt_gamma_energy_mev": 0.0
        },
        "F18": {
            "acolinearity_deg_fwhm": 0.57,
            "half_life_s": 6584.04,
            "k1": 27.9,
            "k2": 2.91,
            "max_range_mm": 3.0,
            "model": "levin_exp",
            "positron_emiss_prob": 0.9686,
            "prob_c": 0.519,
            "prompt_gamma_energy_mev": 0.0
        }}})json";
    stringstream ss(phys_json);
    SourceList sources;

    ASSERT_TRUE(sources.LoadIsotopes(ss));

    const double act = 1.0;
    const double act_uCi = act / Physics::decays_per_microcurie;
    sources.AddSource(std::unique_ptr<Source>(
                new SphereSource({0, 0, 0}, 1.0, act_uCi)));

    ASSERT_TRUE(sources.SetCurIsotope("BackBack", RigidMapR3::Identity()));
    ASSERT_TRUE(sources.SetCurIsotope("C11", RigidMapR3::Identity()));
    ASSERT_TRUE(sources.SetCurIsotope("F18", RigidMapR3::Identity()));


    // BackBack should have been the default
    const Positron& val = dynamic_cast<const Positron&>(
            sources.GetSource(0)->GetIsotope());
    Positron exp(0.0, std::numeric_limits<double>::infinity(), 1.0, 0.0);
    EXPECT_EQ(val, exp);


    // The last positron we set was F18, so check that takes for the next
    // source added.
    sources.AddSource(std::unique_ptr<Source>(
                new SphereSource({0, 0, 0}, 1.0, act_uCi)));
    const Positron& val_f18 = dynamic_cast<const Positron&>(
            sources.GetSource(1)->GetIsotope());
    Positron exp_f18(0.57, 6584.04, 0.9686, 0.0);
    exp_f18.SetPositronRange(0.519, 27.9, 2.91, 3.0);
    EXPECT_EQ(val_f18, exp_f18);
}

TEST(SourceList, ParseBeamIsotope) {
    SourceList sources;
    ASSERT_TRUE(sources.SetCurIsotope("beam 0.0 0.0 1.0 3.0 0.511",
                RigidMapR3::Identity()));

    const double act = 1.0;
    const double act_uCi = act / Physics::decays_per_microcurie;
    sources.AddSource(std::unique_ptr<Source>(
                new SphereSource({0, 0, 0}, 1.0, act_uCi)));


    const Beam* beam = dynamic_cast<const Beam*>(
            &sources.GetSource(0)->GetIsotope());
    ASSERT_NE(beam, nullptr);

    Beam cmp({0.0, 0.0, 1.0}, 3.0, 0.511);
    EXPECT_EQ(cmp, *beam);
}

TEST(SourceList, ParseGaussianBeamIsotope) {
    SourceList sources;
    ASSERT_TRUE(sources.SetCurIsotope("gauss_beam 0.0 0.0 1.0 3.0 0.511",
                RigidMapR3::Identity()));

    const double act = 1.0;
    const double act_uCi = act / Physics::decays_per_microcurie;
    sources.AddSource(std::unique_ptr<Source>(
                new SphereSource({0, 0, 0}, 1.0, act_uCi)));

    const GaussianBeam* beam = dynamic_cast<const GaussianBeam*>(
            &sources.GetSource(0)->GetIsotope());
    ASSERT_NE(beam, nullptr);

    GaussianBeam cmp({0.0, 0.0, 1.0}, 3.0, 0.511);
    EXPECT_EQ(cmp, *beam);
}
