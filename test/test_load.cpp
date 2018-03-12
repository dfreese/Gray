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
#include <string>
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Graphics/ViewableCylinder.h"
#include "Gray/Graphics/ViewableEllipsoid.h"
#include "Gray/Graphics/ViewableParallelepiped.h"
#include "Gray/Graphics/ViewableSphere.h"
#include "Gray/Graphics/ViewableTriangle.h"
#include "Gray/Gray/Config.h"
#include "Gray/Gray/GammaMaterial.h"
#include "Gray/Gray/Load.h"
#include "Gray/Gray/Syntax.h"
#include "Gray/Output/DetectorArray.h"
#include "Gray/Output/Output.h"
#include "Gray/Physics/GammaStats.h"
#include "Gray/Sources/SourceList.h"
#include "Gray/Sources/VectorSource.h"

TEST(LoadTest, HitsFormat) {
    Config config;
    Command cmd("hits_format var_binary");

    // var_ascii is default
    EXPECT_EQ(config.get_format_hits(), Output::Format::VariableAscii);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_format_hits(), Output::Format::VariableBinary);

    cmd = Command("hits_format var_ascii");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("hits_format var_ascii garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("hits_format ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, SinglesFormat) {
    Config config;
    Command cmd("singles_format var_binary");

    // var_ascii is default
    EXPECT_EQ(config.get_format_singles(), Output::Format::VariableAscii);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_format_singles(), Output::Format::VariableBinary);

    cmd = Command("singles_format var_ascii");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("singles_format var_ascii garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("singles_format ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, CoincFormat) {
    Config config;
    Command cmd("coinc_format var_binary");

    // var_ascii is default
    EXPECT_EQ(config.get_format_coinc(), Output::Format::VariableAscii);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_format_coinc(), Output::Format::VariableBinary);

    cmd = Command("coinc_format var_ascii");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("coinc_format var_ascii garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("coinc_format ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, HitsOutput) {
    Config config;
    Command cmd("hits_output test.dat");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_filename_hits(), "test.dat");

    cmd = Command("hits_output test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("hits_output ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, SinglesOutput) {
    Config config;
    Command cmd("singles_output test.dat");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_filename_singles(), "test.dat");

    cmd = Command("singles_output test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("singles_output ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, CoincOutput) {
    Config config;

    Command cmd("coinc_output test.dat");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("coinc_output check.dat");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    std::vector<std::string> exp({"test.dat", "check.dat"});
    EXPECT_EQ(config.get_filenames_coinc(), exp);


    cmd = Command("coinc_output test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("coinc_output ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, ProcessFile) {
    Config config;
    Command cmd("process_file test.dat");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_filename_process(), "test.dat");

    cmd = Command("process_file test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("process_file ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, ProcessLine) {
    Config config;
    Command cmd("process merge detector 10e-9 max");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    cmd = Command("process merge block 10e-9 anger bx by bz");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));

    std::vector<std::string> exp({
            "merge detector 10e-9 max",
            "merge block 10e-9 anger bx by bz"});
    EXPECT_EQ(config.get_process_lines(), exp);

    cmd = Command("process ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, MappingFile) {
    Config config;
    Command cmd("mapping_file test.dat");
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_filename_mapping(), "test.dat");

    cmd = Command("mapping_file test.dat garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("mapping_file ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, HitsVarMask) {
    Config config;
    Command cmd("hits_var_mask 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    // all fields are on by default
    EXPECT_TRUE(config.get_hits_var_output_write_flags().time);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_FALSE(config.get_hits_var_output_write_flags().time);

    cmd = Command("hits_var_mask 0 a 0 0 0 0 0 0 0 0 0 0 0 0 0");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, SinglesVarMask) {
    Config config;
    Command cmd("singles_var_mask 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    // all fields are on by default
    EXPECT_TRUE(config.get_singles_var_output_write_flags().time);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_FALSE(config.get_singles_var_output_write_flags().time);

    cmd = Command("singles_var_mask 0 a 0 0 0 0 0 0 0 0 0 0 0 0 0");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, CoincVarMask) {
    Config config;
    Command cmd("coinc_var_mask 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");

    // all fields are on by default
    EXPECT_TRUE(config.get_coinc_var_output_write_flags().time);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_FALSE(config.get_coinc_var_output_write_flags().time);

    cmd = Command("coinc_var_mask 0 a 0 0 0 0 0 0 0 0 0 0 0 0 0");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, Time) {
    Config config;
    Command cmd("time 100.0");

    EXPECT_EQ(config.get_time(), 0.0);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_time(), 100.0);

    cmd = Command("time 100.0 help");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("time ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, StartTime) {
    Config config;
    Command cmd("start_time 100.0");

    EXPECT_EQ(config.get_start_time(), 0.0);
    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_start_time(), 100.0);

    cmd = Command("start_time 100.0 help");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("start_time ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, Seed) {
    Config config;
    Command cmd("seed 100");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_EQ(config.get_seed(), 100);

    cmd = Command("seed 100 garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));

    cmd = Command("seed ");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_positron) {
    Config config;
    Command cmd("log_positron");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_nuclear_decays());

    cmd = Command("log_positron garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_nondepositing) {
    Config config;
    Command cmd("log_nondepositing");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_nondepositing_inter());

    cmd = Command("log_nondepositing garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_nonsensitive) {
    Config config;
    Command cmd("log_nonsensitive");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_nonsensitive());

    cmd = Command("log_nonsensitive garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_errors) {
    Config config;
    Command cmd("log_errors");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_errors());

    cmd = Command("log_errors garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, log_all) {
    Config config;
    Command cmd("log_all");

    EXPECT_TRUE(Load::ConfigCommand(cmd, config));
    EXPECT_TRUE(config.get_log_all());

    cmd = Command("log_all garbage");
    EXPECT_FALSE(Load::ConfigCommand(cmd, config));
}

TEST(LoadTest, ConfigCommands) {
    Config config;
    std::vector<Command> cmds;
    cmds.emplace_back("seed 100");
    cmds.emplace_back("start_time 5.0");
    cmds.emplace_back("time 25.0");
    cmds.emplace_back("log_all");
    EXPECT_TRUE(Load::ConfigCommands(cmds, config));

    EXPECT_EQ(config.get_seed(), 100);
    EXPECT_EQ(config.get_time(), 25.0);
    EXPECT_EQ(config.get_start_time(), 5.0);
    EXPECT_TRUE(config.get_log_all());
}

TEST(LoadTest, ConfigCommandsFailure) {
    Config config;
    std::vector<Command> cmds;
    cmds.emplace_back("seed 100");
    cmds.emplace_back("start_time 5.0");
    cmds.emplace_back("time 25.0 garbage");
    cmds.emplace_back("log_all");
    EXPECT_FALSE(Load::ConfigCommands(cmds, config));

    EXPECT_EQ(config.get_seed(), 100);
    EXPECT_EQ(config.get_time(), 0.0);
    EXPECT_EQ(config.get_start_time(), 5.0);
    EXPECT_TRUE(config.get_log_all());
}

TEST(LoadTest, ConfigCommandsUnknown) {
    Config config;
    std::vector<Command> cmds;
    cmds.emplace_back("seed 100");
    cmds.emplace_back("start_time 5.0");
    cmds.emplace_back("unknown_time 25.0");
    cmds.emplace_back("log_all");
    EXPECT_TRUE(Load::ConfigCommands(cmds, config));

    EXPECT_EQ(config.get_seed(), 100);
    EXPECT_EQ(config.get_time(), 0.0);
    EXPECT_EQ(config.get_start_time(), 5.0);
    EXPECT_TRUE(config.get_log_all());
}


class SceneLoadTest : public ::testing::Test {
public:
    Config config;
    SourceList sources;
    SceneDescription scene;
    DetectorArray det_array;
protected:
    virtual void SetUp() {
        scene.AddMaterial(std::unique_ptr<GammaMaterial>(new GammaMaterial(
                   0, "world", false, false, GammaStats())));
        scene.AddMaterial(std::unique_ptr<GammaMaterial>(new GammaMaterial(
                   1, "default", false, true, GammaStats())));
        scene.AddMaterial(std::unique_ptr<GammaMaterial>(new GammaMaterial(
                   2, "sensitive", true, true, GammaStats())));
        scene.SetDefaultMaterial("default");
        sources.AddIsotope("BackBack", std::unique_ptr<Positron>(
                    new Positron(0.0, 1000.0, 0.0, 1.0)));
        sources.AddIsotope("F18", std::unique_ptr<Positron>(
                    new Positron(0.57, 6584.04, 0.0, 1.0)));
        sources.SetCurIsotope("BackBack", RigidMapR3::Identity());
    }
};

TEST_F(SceneLoadTest, SceneDefault) {
    EXPECT_EQ(scene.GetDefaultMaterial().GetName(), "default");
}

TEST_F(SceneLoadTest, SceneCommandsConfigCommands) {
    std::vector<Command> cmds;
    cmds.emplace_back("seed 100");
    cmds.emplace_back("start_time 5.0");
    cmds.emplace_back("log_all");
    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));

    EXPECT_EQ(config.get_seed(), 100);
    EXPECT_EQ(config.get_time(), 0.0);
    EXPECT_EQ(config.get_start_time(), 5.0);
    EXPECT_TRUE(config.get_log_all());
}

TEST_F(SceneLoadTest, SceneCommandsAnnCyl) {
    std::vector<Command> cmds;
    cmds.emplace_back("ann_cyl 0.0 0.0 0.0 0.0 0.0 1.0 30.0 50.0 10.0");
    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));

    // ann_cyl creates 800 triangles to mimic the donut form.
    size_t size = scene.NumViewables();
    EXPECT_EQ(size, 800);
}

TEST_F(SceneLoadTest, SceneCommandsEllipticCyl) {
    std::vector<Command> cmds;
    cmds.emplace_back("elliptic_cyl 0.0 0.0 0.0 0.0 0.0 1.0 30.0 50.0 10.0");
    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));

    ASSERT_EQ(scene.NumViewables(), 1);

    //ViewableEllipsoid& ve;
    ASSERT_TRUE(dynamic_cast<ViewableCylinder*>(&scene.GetViewable(0)));
    ViewableCylinder& ve = dynamic_cast<ViewableCylinder&>(
            scene.GetViewable(0));
    EXPECT_EQ(ve.GetRadiusA(), 50.0);
    EXPECT_EQ(ve.GetRadiusB(), 30.0);
}

TEST_F(SceneLoadTest, SceneCommandsCyl) {
    std::vector<Command> cmds;
    cmds.emplace_back("scale_act 2");
    cmds.emplace_back("cyl 0.0 0.0 0.0 0.0 0.0 1.0 30.0 50.0");
    cmds.emplace_back("cyl_src 0.0 0.0 0.0 0.0 0.0 1.0 30.0 50.0 10.0");
    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));
    EXPECT_EQ(sources.NumSources(), 1);
    auto src = sources.GetSource(0);
    ASSERT_TRUE(src);
    ASSERT_TRUE(src->Inside({0.0, 30.0, 25.0}));
    // Files are in uCi, internal is in Bq
    ASSERT_EQ(src->GetActivity(), 20.0 * 37000);
    ASSERT_EQ(scene.NumViewables(), 1);
    ViewableCylinder& ve = dynamic_cast<ViewableCylinder&>(
            scene.GetViewable(0));
    EXPECT_EQ(ve.GetRadiusA(), 30.0);
    EXPECT_EQ(ve.GetHeight(), 50.0);
}

TEST_F(SceneLoadTest, SceneCommandsSphere) {
    std::vector<Command> cmds;
    cmds.emplace_back("scale_act 2");
    cmds.emplace_back("sphere 0.0 0.0 0.0 1.0");
    cmds.emplace_back("sp_src 1.0 1.0 1.0 1.0 10.0");
    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));
    EXPECT_EQ(sources.NumSources(), 1);
    auto src = sources.GetSource(0);
    ASSERT_TRUE(src);
    ASSERT_FALSE(src->Inside({1.0, 2.0, 1.0}));
    ASSERT_TRUE(src->Inside({1.0, 2.0 - 1e-6, 1.0}));
    // Files are in uCi, internal is in Bq
    ASSERT_EQ(src->GetActivity(), 20.0 * 37000);
    ASSERT_EQ(scene.NumViewables(), 1);
    ViewableSphere & ve = dynamic_cast<ViewableSphere&>(
            scene.GetViewable(0));
    EXPECT_EQ(ve.GetRadius(), 1.0);
}

TEST_F(SceneLoadTest, SceneCommandsRectangle) {
    std::vector<Command> cmds;
    cmds.emplace_back("k 0.0 0.0 0.0 1.0 1.0 1.0");

    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));
    ASSERT_EQ(scene.NumViewables(), 1);
    ViewableParallelepiped& v = dynamic_cast<ViewableParallelepiped&>(
            scene.GetViewable(0));
    EXPECT_EQ(v.GetVertexA(), VectorR3(-0.5, -0.5, -0.5));
    EXPECT_EQ(v.GetVertexB(), VectorR3(-0.5, 0.5, -0.5));
    EXPECT_EQ(v.GetVertexC(), VectorR3(-0.5, -0.5, 0.5));
    EXPECT_EQ(v.GetVertexD(), VectorR3(0.5, -0.5, -0.5));
    EXPECT_EQ(v.GetDetectorId(), -1);
}

TEST_F(SceneLoadTest, SceneCommandsRectangleSensitive) {
    std::vector<Command> cmds;
    cmds.emplace_back("m sensitive");
    cmds.emplace_back("k 0.0 0.0 0.0 1.0 1.0 1.0");

    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));
    ASSERT_EQ(scene.NumViewables(), 1);
    ViewableParallelepiped& v = dynamic_cast<ViewableParallelepiped&>(
            scene.GetViewable(0));
    EXPECT_EQ(v.GetVertexA(), VectorR3(-0.5, -0.5, -0.5));
    EXPECT_EQ(v.GetVertexB(), VectorR3(-0.5, 0.5, -0.5));
    EXPECT_EQ(v.GetVertexC(), VectorR3(-0.5, -0.5, 0.5));
    EXPECT_EQ(v.GetVertexD(), VectorR3(0.5, -0.5, -0.5));
    EXPECT_EQ(v.GetDetectorId(), 0);
}

TEST_F(SceneLoadTest, SceneCommandsArray) {
    std::vector<Command> cmds;
    cmds.emplace_back("array 0.0 0.0 0.0 1 3 3 1.1 1.1 1.1 1.0 1.0 1.0");

    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));
    ASSERT_EQ(scene.NumViewables(), 9);
    ViewableParallelepiped& v = dynamic_cast<ViewableParallelepiped&>(
            scene.GetViewable(4));
    EXPECT_EQ(v.GetVertexA(), VectorR3(-0.5, -0.5, -0.5));
    EXPECT_EQ(v.GetVertexB(), VectorR3(-0.5, 0.5, -0.5));
    EXPECT_EQ(v.GetVertexC(), VectorR3(-0.5, -0.5, 0.5));
    EXPECT_EQ(v.GetVertexD(), VectorR3(0.5, -0.5, -0.5));
    EXPECT_EQ(v.GetDetectorId(), -1);
}

TEST_F(SceneLoadTest, SceneCommandsArraySensitive) {
    std::vector<Command> cmds;
    cmds.emplace_back("m sensitive");
    cmds.emplace_back("array 0.0 0.0 0.0 1 3 3 1.1 1.1 1.1 1.0 1.0 1.0");

    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));
    ASSERT_EQ(scene.NumViewables(), 9);
    ViewableParallelepiped& v = dynamic_cast<ViewableParallelepiped&>(
            scene.GetViewable(4));
    EXPECT_EQ(v.GetDetectorId(), 4);
}
TEST_F(SceneLoadTest, SceneCommandsPolygon) {
    std::vector<Command> cmds;
    cmds.emplace_back("p 3");
    cmds.emplace_back("0.0 0.0 0.0");
    cmds.emplace_back("1.0 0.0 0.0");
    cmds.emplace_back("0.0 1.0 0.0");

    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));
    ASSERT_EQ(scene.NumViewables(), 1);
    EXPECT_EQ(scene.GetViewable(0).GetDetectorId(), -1);
    auto v = dynamic_cast<ViewableTriangle&>(scene.GetViewable(0));
    EXPECT_EQ(v.GetVertexA(), VectorR3(0.0, 0.0, 0.0));
    EXPECT_EQ(v.GetVertexB(), VectorR3(1.0, 0.0, 0.0));
    EXPECT_EQ(v.GetVertexC(), VectorR3(0.0, 1.0, 0.0));
}

TEST_F(SceneLoadTest, SceneCommandsVectorSource) {
    std::vector<Command> cmds;
    cmds.emplace_back("scale_act 2");
    cmds.emplace_back("start_vecsrc 10");
    cmds.emplace_back("p 3");
    cmds.emplace_back("0.0 0.0 0.0");
    cmds.emplace_back("1.0 0.0 0.0");
    cmds.emplace_back("0.0 1.0 0.0");
    cmds.emplace_back("end_vecsrc");

    Load load;
    EXPECT_TRUE(load.SceneCommands(cmds, sources, scene, det_array, config));
    ASSERT_EQ(scene.NumViewables(), 0);
    EXPECT_EQ(sources.NumSources(), 1);
    auto src = sources.GetSource(0);
    ASSERT_TRUE(src);
    // Files are in uCi, internal is in Bq
    ASSERT_EQ(src->GetActivity(), 20.0 * 37000);
    EXPECT_NE(dynamic_cast<VectorSource const*>(src.get()), nullptr);
}
