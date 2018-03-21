/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */
#ifndef LOAD_H
#define LOAD_H
#include <memory>
#include <stack>
#include <vector>
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Graphics/ViewableTriangle.h"
#include "Gray/Gray/Syntax.h"
#include "Gray/VrMath/LinearR3.h"

class Config;
class DetectorArray;
class SourceList;
class GammaMaterial;

class Load {
public:
    Load();
    bool File(
            const std::string& filename,
            SourceList& sources,
            SceneDescription& scene,
            DetectorArray& det_array,
            Config& config);
    bool ConfigFile(const std::string& filename, Config& config);
    static bool ConfigCommands(std::vector<Command>& cmds, Config& config);
    static bool ConfigCommand(
            Command& cmd, Config & config, bool reject_unknown = false);
    bool SceneCommands(
            std::vector<Command>& cmds,
            SourceList& list,
            SceneDescription& scene,
            DetectorArray& det_array,
            Config& config);
    bool SceneCommand(
            Command& cmd,
            SourceList& list,
            SceneDescription& scene,
            DetectorArray& det_array,
            Config& config);
    void SetCameraView(SceneDescription& scene);
    void ProcessPolygonVerts(SceneDescription& scene);
    static std::vector<ViewableTriangle> MakeAnnulusCylinder(
            const VectorR3 & center, const VectorR3 & axis,
            double radius_inner, double radius_outer, double width,
            int det_id, Material * material);
    static std::vector<ViewableTriangle> MakeAnnulusCylinder(
            double radius_inner, double radius_outer, double width);
    static void DisableRayleigh(SceneDescription& scene);

private:
    VectorR3 up = {0, 1, 0};
    VectorR3 from;
    VectorR3 at = {0, 0, 0};
    double fov_angle_deg = 35.0;
    bool view_pos_set = false;
    bool lookat_pos_set = false;
    bool load_polygon_lines = false;
    int no_polygon_verts = 0;
    int polygon_det_id = -1;
    std::vector<VectorR3> polygon_verts;
    bool load_vector_source = false;
    double polygon_scale = 1.0;
    int block_id = 0;
    std::stack<RigidMapR3> matrix_stack;
    double vector_source_activity = -1;
    std::unique_ptr<SceneDescription> vector_source_scene;
    double activity_scale = 1.0;
    GammaMaterial* cur_material = nullptr;
};

#endif // LOAD_H
