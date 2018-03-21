/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Gray/Load.h"
#include <array>
#include <memory>
#include <vector>
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Graphics/TransformViewable.h"
#include "Gray/Graphics/ViewableCylinder.h"
#include "Gray/Graphics/ViewableEllipsoid.h"
#include "Gray/Graphics/ViewableParallelepiped.h"
#include "Gray/Graphics/ViewableSphere.h"
#include "Gray/Graphics/ViewableTriangle.h"
#include "Gray/Gray/Config.h"
#include "Gray/Gray/File.h"
#include "Gray/Gray/GammaMaterial.h"
#include "Gray/Gray/String.h"
#include "Gray/Gray/Syntax.h"
#include "Gray/Output/DetectorArray.h"
#include "Gray/Sources/AnnulusCylinderSource.h"
#include "Gray/Sources/AnnulusEllipticCylinderSource.h"
#include "Gray/Sources/CylinderSource.h"
#include "Gray/Sources/EllipsoidSource.h"
#include "Gray/Sources/EllipticCylinderSource.h"
#include "Gray/Sources/PointSource.h"
#include "Gray/Sources/SourceList.h"
#include "Gray/Sources/SphereSource.h"
#include "Gray/Sources/RectSource.h"
#include "Gray/Sources/VectorSource.h"
#include "Gray/Sources/VoxelSource.h"
#include "Gray/VrMath/LinearR3.h"

Load::Load() :
    matrix_stack({RigidMapR3::Identity()})
{
}

bool Load::ConfigCommand(Command& cmd, Config & config, bool reject_unknown) {
    if (cmd == "hits_format") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for hits_format");
            return (false);
        } else if (!config.set_format_hits(cmd.Join())) {
            cmd.MarkError("Invalid format identifier: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "singles_format") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for singles_format");
            return (false);
        } else if (!config.set_format_singles(cmd.Join())) {
            cmd.MarkError("Invalid format identifier: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "coinc_format") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for coinc_format");
            return (false);
        } else if (!config.set_format_coinc(cmd.Join())) {
            cmd.MarkError("Invalid format identifier: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "hits_output") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for hits_output");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for hits_output");
            return (false);
        }
        config.set_filename_hits(cmd.Join());
        return (true);
    } else if (cmd == "singles_output") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for singles_output");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for singles_output");
            return (false);
        }
        config.set_filename_singles(cmd.Join());
        return (true);
    } else if (cmd == "coinc_output") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for coinc_output");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for coinc_output");
            return (false);
        }
        config.add_filename_coinc(cmd.Join());
        return (true);
    } else if (cmd == "process_file") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for process_file");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for process_file");
            return (false);
        }
        // Make the file relative to whichever file in which the command was
        // placed
        std::string filename = File::Join(File::Dir(cmd.filename), cmd.Join());
        config.set_filename_process(filename);
        return (true);
    }  else if (cmd == "process") {
        if (cmd.tokens.size() == 1) {
            cmd.MarkError("No options given for process");
            return (false);
        }
        // Any line prefaced with pipeilne_config will be processed as a
        // pipeline file.
        config.add_process_line(cmd.Join());
        return (true);
    } else if (cmd == "mapping_file") {
        if (cmd.tokens.size() < 2) {
            cmd.MarkError("No filename specified for mapping_file");
            return (false);
        } else if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for mapping_file");
            return (false);
        }
        // Make the file relative to whichever file in which the command was
        // placed
        std::string filename = File::Join(File::Dir(cmd.filename), cmd.Join());
        config.set_filename_mapping(filename);
        return (true);
    } else if (cmd == "hits_var_mask") {
        if (!config.set_hits_var_output_write_flags(cmd.Join())) {
            cmd.MarkError("Invalid variable mask: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "singles_var_mask") {
        if (!config.set_singles_var_output_write_flags(cmd.Join())) {
            cmd.MarkError("Invalid variable mask: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "coinc_var_mask") {
        if (!config.set_coinc_var_output_write_flags(cmd.Join())) {
            cmd.MarkError("Invalid variable mask: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "time") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for time");
            return (false);
        }
        // simulation time in seconds
        double sim_time;
        if (!cmd.parse(sim_time)) {
            cmd.MarkError("Invalid simulation time: " + cmd.Join());
            return (false);
        }
        config.set_time(sim_time);
        return (true);
    } else if (cmd == "start_time") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for start time");
            return (false);
        }
        // time in seconds
        double start_time = 0;
        if (!cmd.parse(start_time)) {
            cmd.MarkError("Invalid start time: " + cmd.Join());
            return (false);
        }
        config.set_start_time(start_time);
        return (true);
    } else if (cmd == "seed") {
        if (cmd.tokens.size() > 2) {
            cmd.MarkError("Too many options for seed");
            return (false);
        }
        unsigned long seed = 1;
        if (!cmd.parse(seed)) {
            cmd.MarkError("Invalid seed: " + cmd.Join());
            return (false);
        }
        config.set_seed(seed);
        return (true);
    } else if (cmd == "log_positron") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_positron: " +
                    cmd.Join());
            return (false);
        }
        config.set_log_nuclear_decays(true);
        return (true);
    } else if (cmd == "log_nondepositing") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_nondepositing: " +
                    cmd.Join());
            return (false);
        }
        config.set_log_nondepositing_inter(true);
        return (true);
    } else if (cmd == "log_nonsensitive") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_nonsensitive: " +
                    cmd.Join());
            return (false);
        }
        config.set_log_nonsensitive(true);
        return (true);
    } else if (cmd == "log_errors") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_errors: " +
                    cmd.Join());
            return (false);
        }
        config.set_log_errors(true);
        return (true);
    } else if (cmd == "log_all") {
        if (cmd.tokens.size() > 1) {
            cmd.MarkError("Unrecognized options after log_all: " + cmd.Join());
            return (false);
        }
        config.set_log_all(true);
        return (true);
    } else {
        // Ignore other commands.
        return (!reject_unknown);
    }
}

bool Load::ConfigCommands(std::vector<Command>& cmds, Config& config) {
    bool result = true;
    for (Command& cmd : cmds) {
        result &= Load::ConfigCommand(cmd, config, /*reject_unknown = */false);
    }
    return (result);
}

bool Load::SceneCommands(
        std::vector<Command>& cmds,
        SourceList& sources,
        SceneDescription& scene,
        DetectorArray& det_array,
        Config& config)
{
    cur_material = static_cast<GammaMaterial*>(&scene.GetDefaultMaterial());
    bool result = true;
    for (Command& cmd : cmds) {
        result &= Load::SceneCommand(cmd, sources, scene, det_array, config);
        if (cmd.IsError()) {
            std::cerr << "Error in " << cmd.filename << ", line "
                << cmd.line << ":\n\"" << cmd.Original() << "\"\n"
                << cmd.ErrorMsg() << "\n";
        } else if (cmd.IsWarning()) {
            std::cout << "Warning in " << cmd.filename << ", line "
                << cmd.line << ": " << cmd.WarningMsg() << "\n";
        }
    }
    return (result);
}

bool Load::ConfigFile(const std::string& filename, Config& config) {
    auto cmds = Syntax::ParseCommands(filename);
    return (ConfigCommands(cmds, config));
}

bool Load::File(
        const std::string& filename,
        SourceList& sources,
        SceneDescription& scene,
        DetectorArray& det_array,
        Config& config)
{
    auto cmds = Syntax::ParseCommands(filename);
    if (SceneCommands(cmds, sources, scene, det_array, config)) {
        SetCameraView(scene);
        return (true);
    } else {
        return (false);
    }
}

bool Load::SceneCommand(
        Command& cmd,
        SourceList& sources,
        SceneDescription& scene,
        DetectorArray& det_array,
        Config& config)
{
    if (!cur_material) {
        cur_material = static_cast<GammaMaterial*>(
                &scene.GetDefaultMaterial());
    }
    const RigidMapR3& cur_matrix = matrix_stack.top();
    if (load_polygon_lines) {
        VectorR3 point;
        bool status = cmd.parseAll(point.x, point.y, point.z);
        // Always add a point the vertices, so we don't get out of sync with
        // the input file.
        polygon_verts.push_back(point);
        if (!status) {
            cmd.MarkError("vertex point format: [x] [y] [z]");
            return (true);
        }
        if ((int)polygon_verts.size() == no_polygon_verts) {
            // Choose if we're adding this to the vector source's scene or
            // the geometric scene.
            SceneDescription & local_scene = (load_vector_source ?
                    (*vector_source_scene.get()):scene);
            ProcessPolygonVerts(local_scene);
        }
        return (true);
    } else if (cmd == "disable_half_life") {
        if (!cmd.parse()) {
            cmd.MarkError("disable_half_life takes no options");
            return (false);
        }
        sources.DisableHalfLife();
        return (true);
    } else if (cmd == "ann_cyl") {
        VectorR3 center, axis;
        double inner_radius, outer_radius, height;
        if (!cmd.parse(center.x, center.y, center.z,
                    axis.x, axis.y, axis.z,
                    outer_radius, inner_radius, height))
        {
            cmd.MarkError("format: ann_cyl [center xyz] [axis xyz]"
                    " [outer_radius] [inner_radius] [height]");
            return (false);
        }
        int det_id = -1;
        auto pieces = Load::MakeAnnulusCylinder(
                center, axis, inner_radius, outer_radius, height, det_id,
                cur_material);
        for (const auto & triangle: pieces) {
            std::unique_ptr<ViewableTriangle> vc(
                    new ViewableTriangle(triangle));
            TransformWithRigid(vc.get(), cur_matrix);
            scene.AddViewable(std::move(vc));
        }
        return (true);
    } else if (cmd == "elliptic_cyl") {
        VectorR3 center, axis;
        double radius1, radius2, height;
        if (!cmd.parse(center.x, center.y, center.z,
                    axis.x, axis.y, axis.z,
                    radius1, radius2, height))
        {
            cmd.MarkError("format: elliptic_cyl [center xyz] [axis xyz]"
                    " [radius1] [radius2] [height]");
            return (false);
        }
        std::unique_ptr<ViewableCylinder> vc(new ViewableCylinder());
        vc->SetCenterAxis(axis);
        vc->SetCenter(center);
        vc->SetRadii(radius2, radius1);
        vc->SetHeight(height);
        vc->SetMaterial(cur_material);
        TransformWithRigid(vc.get(), cur_matrix);
        scene.AddViewable(std::move(vc));
        return (true);
    } else if (cmd == "isotope") {
        // The isotope command can create new isotopes, currently only a beam
        // isotope, so pass along the rest of the command to SourceList.
        if (!sources.SetCurIsotope(cmd.Join(), cur_matrix)) {
            cmd.MarkError("Invalid isotope: \"" + cmd.Join() + "\"");
            return(false);
        }
        return (true);
    } else if (cmd == "cyl_src") {
        VectorR3 center, axis;
        double radius, height, activity;
        if (!cmd.parse(center.x, center.y, center.z,
                    axis.x, axis.y, axis.z, radius, height, activity))
        {
            cmd.MarkError("format: cyl_src  [center xyz] [axis xyz]"
                    "[radius] [height] [activity]");
            return (false);
        }
        cur_matrix.Transform(&center);
        cur_matrix.Transform3x3(&axis);
        std::unique_ptr<CylinderSource> cyl(new CylinderSource(
                    center, radius, height, axis, activity_scale * activity));
        sources.AddSource(std::move(cyl));
        return (true);
    } else if (cmd == "sp_src") {
        VectorR3 center;
        double radius, activity;
        if (!cmd.parse(center.x, center.y, center.z, radius, activity)) {
            cmd.MarkError("format: sp_src [center xyz] [radius] [activity]");
            return (false);
        }
        cur_matrix.Transform(&center);
        std::unique_ptr<SphereSource> s(new SphereSource(
                    center, radius, activity_scale * activity));
        sources.AddSource(std::move(s));
        return (true);
    } else if (cmd == "rect_src") {
        VectorR3 center, size, axis;
        double activity;
        if (!cmd.parse(center.x, center.y, center.z,
                    size.x, size.y, size.z,
                    axis.x, axis.y, axis.z, activity))
        {
            cmd.MarkError(
                    "rect_src format [center xyz] [size xyz] [axis xyz] [act]");
            return (false);
        }
        cur_matrix.Transform(&center);
        cur_matrix.Transform3x3(&axis);
        std::unique_ptr<RectSource> s(new RectSource(
                    center, size, axis, activity_scale * activity));
        sources.AddSource(std::move(s));
        return (true);
    } else if (cmd == "pt_src") {
        // pt_src pos_x pos_y pos_z activity
        VectorR3 center;
        double activity;
        if (!cmd.parse(center.x, center.y, center.z, activity)) {
            cmd.MarkError("format: pt_src [center xyz] [activity]");
            return (false);
        }
        cur_matrix.Transform(&center);
        std::unique_ptr<PointSource> s(new PointSource(
                    center, activity_scale * activity));
        sources.AddSource(std::move(s));
        return (true);
    } else if (cmd == "voxel_src") {
        std::string filename;
        VectorR3 center;
        VectorR3 size;
        // Assume z aligns with z for right now.
        VectorR3 axis(0, 0, 1);
        double activity;
        if (!cmd.parse(center.x, center.y, center.z,
                    size.x, size.y, size.z, filename, activity))
        {
            cmd.MarkError("format: voxel_src [center xyz] [size xyz]"
                    " [filename] [activity]");
            return (false);
        }
        // Make the file relative to whichever file in which the command was
        // placed
        filename = File::Join(File::Dir(cmd.filename), filename);
        std::unique_ptr<VoxelSource> s(new VoxelSource(
                    center, size, axis, activity_scale * activity));
        if (!s->Load(filename)) {
            cmd.MarkError("Unable to load image: " + filename);
            return(false);
        }
        sources.AddSource(std::move(s));
        return (true);
    } else if (cmd == "ellipsoid_src") {
        VectorR3 center, axis1, axis2;
        double radius1, radius2, radius3, activity;
        if (!cmd.parse(center.x, center.y, center.z,
                    axis1.x, axis1.y, axis1.z,
                    axis2.x, axis2.y, axis2.z,
                    radius1, radius2, radius3, activity))
        {
            cmd.MarkError("Invalid values for elliptic_cyl_src");
            return (false);
        }
        cur_matrix.Transform(&center);
        cur_matrix.Transform3x3(&axis1);
        cur_matrix.Transform3x3(&axis2);
        std::unique_ptr<EllipsoidSource> ve(new EllipsoidSource(
                    center, axis1, axis2, radius1, radius2, radius3,
                    activity_scale * activity));
        sources.AddSource(std::move(ve));
        return (true);
    } else if (cmd == "elliptic_cyl_src") {
        VectorR3 center, axis;
        double radius1, radius2, height, activity;
        if (!cmd.parse(center.x, center.y, center.z,
                    axis.x, axis.y, axis.z,
                    radius1, radius2, height, activity))
        {
            cmd.MarkError("Invalid values for elliptic_cyl_src");
            return (false);
        }
        cur_matrix.Transform(&center);
        cur_matrix.Transform3x3(&axis);
        std::unique_ptr<EllipticCylinderSource> cyl(
                new EllipticCylinderSource(
                    center, radius1, radius2, height,
                    axis, activity_scale * activity));
        sources.AddSource(std::move(cyl));
        return (true);
    } else if (cmd == "annulus_ell_src") {
        VectorR3 center, axis;
        double radius1, radius2, height, activity;
        if (!cmd.parse(center.x, center.y, center.z,
                    axis.x, axis.y, axis.z,
                    radius1, radius2, height, activity))
        {
            cmd.MarkError("Invalid values for annulus_ell_src");
            return (false);
        }
        cur_matrix.Transform(&center);
        cur_matrix.Transform3x3(&axis);
        std::unique_ptr<AnnulusEllipticCylinderSource> cyl(
                new AnnulusEllipticCylinderSource(
                    center, radius1, radius2, height,
                    axis, activity_scale * activity));
        sources.AddSource(std::move(cyl));
        return (true);
    } else if (cmd == "annulus_cyl_src") {
        if (cmd.tokens.size() != 10) {
            cmd.MarkError("Invalid syntax for annulus_cyl_src");
            return (false);
        }
        VectorR3 center, axis;
        double radius, height, activity;
        if (!cmd.parse(center.x, center.y, center.z,
                    axis.x, axis.y, axis.z, radius, height, activity))
        {
            cmd.MarkError("Invalid values for annulus_cyl_src");
            return (false);
        }
        cur_matrix.Transform(&center);
        cur_matrix.Transform3x3(&axis);
        std::unique_ptr<AnnulusCylinderSource> cyl(new AnnulusCylinderSource(
                    center, radius, height, axis, activity_scale * activity));
        sources.AddSource(std::move(cyl));
        return (true);
    } else if (cmd == "scale_act") {
        if (cmd.tokens.size() != 2) {
            cmd.MarkError("Invalid syntax for scale_act");
            return (false);
        }
        if (!cmd.parse(activity_scale)) {
            cmd.MarkError("Invalid scale value: " + cmd.Join());
            return (false);
        }
        return (true);
    } else if (cmd == "echo") {
        std::cout << "echo: " << cmd.Join() << "\n";
        return (true);
    } else if (cmd == "m") {
        std::string mat_name;
        if (!cmd.parse(mat_name)) {
            cmd.MarkError("format: m [material name]");
            return (false);
        } else if (!scene.HasMaterial(mat_name)) {
            cmd.MarkError("Invalid material: " + mat_name);
            return(false);
        }
        cur_material = static_cast<GammaMaterial*>(
                &scene.GetMaterial(mat_name));
        return (true);
    } else if (cmd == "disable_rayleigh") {
        if (!cmd.parse()) {
            cmd.MarkError("disable_rayleigh takes no options");
            return (false);
        }
        Load::DisableRayleigh(scene);
        return (true);
    } else if (cmd == "t") {
        VectorR3 trans;
        if (!cmd.parse(trans.x, trans.y, trans.z)) {
            cmd.MarkError("format: t [shift xyz]");
            return (false);
        }
        // This doesn't seem to be the most obvious, or correct way to apply
        // a translation to the matrix, so perhaps check this.  It seems to
        // work.
        matrix_stack.top().Transform3x3(&trans);
        matrix_stack.top().ApplyTranslationLeft(trans);
        return (true);
    } else if (cmd == "push") {
        if (!cmd.parse()) {
            cmd.MarkError("push takes no options");
            return (false);
        }
        matrix_stack.emplace(cur_matrix);
        return (true);
    } else if (cmd == "pop") {
        if (!cmd.parse()) {
            cmd.MarkError("pop takes no options");
            return (false);
        }
        // The default matrix is placed at the bottom of the stack.
        // We should never pop that off.
        if (matrix_stack.size() <= 1) {
            cmd.MarkError("unpaired pop found");
            return (false);
        }
        matrix_stack.pop();
        return (true);
    } else if (cmd == "raxis") {
        VectorR3 axis;
        double degree;
        if (!cmd.parse(axis.x, axis.y, axis.z, degree)) {
            cmd.MarkError("format: raxis [axis xyz] [angle (deg)]");
            return (false);
        }
        const double theta = degree * (M_PI / 180.0);
        matrix_stack.top() *= VrRotate(theta, axis);
        return (true);
    } else if (cmd == "sphere") {
        VectorR3 center;
        double radius;
        if (!cmd.parse(center.x, center.y, center.z, radius)) {
            cmd.MarkError("sphere format: [center xyz] [radius]");
            return (false);
        }
        std::unique_ptr<ViewableSphere> s(new ViewableSphere(center, radius));
        s->SetMaterial(cur_material);
        TransformWithRigid(s.get(), cur_matrix);
        scene.AddViewable(std::move(s));
        return (true);
    } else if (cmd == "cyl") {
        VectorR3 center, axis;
        double radius, height;
        if (!cmd.parse(center.x, center.y, center.z,
                   axis.x, axis.y, axis.z, radius, height))
        {
            cmd.MarkError("cyl format [center xyz] [axis xyz]"
                    " [radius] [height]");
            return (false);
        }
        std::unique_ptr<ViewableCylinder> vc(new ViewableCylinder());
        vc->SetRadius(radius);
        vc->SetCenterAxis(axis);
        vc->SetCenter(center);
        vc->SetHeight(height);
        vc->SetMaterial(cur_material);
        TransformWithRigid(vc.get(), cur_matrix);
        scene.AddViewable(std::move(vc));
        return (true);
    } else if (cmd == "ellipsoid") {
        VectorR3 center, axis1, axis2;
        double radius1, radius2, radius3;
        if (!cmd.parse(center.x, center.y, center.z,
                    axis1.x, axis1.y, axis1.z,
                    axis2.x, axis2.y, axis2.z,
                    radius1, radius2, radius3))
        {
            cmd.MarkError("Invalid values for ellipsoid");
            return (false);
        }
        std::unique_ptr<ViewableEllipsoid> ve(new ViewableEllipsoid());
        ve->SetCenter(center);
        ve->SetAxes(axis1, axis2);
        ve->SetRadii(radius3, radius2, radius1);
        ve->SetMaterial(cur_material);
        TransformWithRigid(ve.get(), cur_matrix);
        scene.AddViewable(std::move(ve));
        return (true);
    } else if (cmd == "k") {
        VectorR3 center, size;
        if (!cmd.parse(center.x, center.y, center.z, size.x, size.y, size.z)) {
            cmd.MarkError("k format: [center xyz] [size xyz]");
            return (false);
        }

        int det_id = -1;
        if (cur_material->IsSensitive()) {
            det_id = det_array.AddDetector(
                    center, size, cur_matrix, 0, 0, 0, block_id);
            ++block_id;
        }
        std::unique_ptr<ViewableParallelepiped> vp(new ViewableParallelepiped(
                    center, size));
        vp->SetMaterial(cur_material);
        vp->SetDetectorId(det_id);
        TransformWithRigid(vp.get(), cur_matrix);
        scene.AddViewable(std::move(vp));
        return (true);
    } else if (cmd == "array") {
        VectorR3 center, size, step;
        std::array<int, 3> dims;
        if (!cmd.parse(center.x, center.y, center.z,
                    dims[0], dims[1], dims[2],
                    step.x, step.y, step.z,
                    size.x, size.y, size.z))
        {
            cmd.MarkError("array format: [center xyz] [no steps xyz]"
                    "[pitch xyz] [size xyz]");
            return (false);
        }

        for (int k = 0; k < dims[2]; k++) {
            for (int j = 0; j < dims[1]; j++) {
                for (int i = 0; i < dims[0]; i++) {
                    int det_id = -1;
                    VectorR3 local_center(
                            center.x - (dims[0] - 1) * step.x / 2.0,
                            center.y - (dims[1] - 1) * step.y / 2.0,
                            center.z - (dims[2] - 1) * step.z / 2.0);
                    local_center.x += i * step.x;
                    local_center.y += j * step.y;
                    local_center.z += k * step.z;

                    if (cur_material->IsSensitive()) {
                        det_id = det_array.AddDetector(
                                local_center, size, cur_matrix,
                                i, j, k, block_id);
                    }

                    std::unique_ptr<ViewableParallelepiped> vp(
                            new ViewableParallelepiped(local_center, size));
                    vp->SetMaterial(cur_material);
                    vp->SetDetectorId(det_id);
                    TransformWithRigid(vp.get(), cur_matrix);
                    scene.AddViewable(std::move(vp));
                }
            }
        }
        if (cur_material->IsSensitive()) {
            ++block_id;
        }
        return (true);
    } else if (cmd == "p") {
        int num_verts;
        if (!cmd.parse(num_verts)) {
            cmd.MarkError("Invalid number of vertices");
            return(false);
        }
        if (num_verts < 3) {
            cmd.MarkError("Number of vertices is less than 3");
            return(false);
        }
        if (num_verts > 256) {
            cmd.MarkError("More than 256 vertices is not supported");
            return(false);
        }
        load_polygon_lines = true;
        no_polygon_verts = num_verts;
        polygon_verts.clear();
        return (true);
    } else if (cmd == "scale") {
        if (!cmd.parse(polygon_scale)) {
            cmd.MarkError("invalid scale value");
        }
        return (true);
    } else if (cmd == "increment") {
        if (!cmd.parse()) {
            cmd.MarkError("increment takes no options");
        }
        polygon_det_id = det_array.AddDetector(
                {0, 0, 0}, {1, 1, 1}, cur_matrix, 0, 0, 0, block_id++);
        return (true);
    } else if (cmd == "start_vecsrc") {
        if (!cmd.parse(vector_source_activity)) {
            cmd.MarkError("Invalid vector source activity");
            return (false);
        }
        if (vector_source_scene) {
            cmd.MarkError("previous start_vecsrc left unpaired");
            return (false);
        }
        load_vector_source = true;
        vector_source_scene = unique_ptr<SceneDescription>(
                new SceneDescription());
        return (true);
    } else if (cmd == "end_vecsrc") {
        if (!cmd.parse()) {
            vector_source_scene = nullptr;
            cmd.MarkError("end_vecsrc requires no options");
            return (false);
        }
        if (!vector_source_scene) {
            cmd.MarkError("unpaired end_vecsrc found");
            return (false);
        }
        sources.AddSource(std::unique_ptr<Source>(new VectorSource(
                        activity_scale * vector_source_activity,
                        std::move(vector_source_scene))));
        load_vector_source = false;
        vector_source_activity = -1;
        return (true);
    } else if (cmd == "from") {
        if (!cmd.parse(from.x, from.y, from.z)) {
            cmd.MarkError("Invalid values for from vector");
            return (false);
        }
        view_pos_set = true;
        return (true);
    } else if (cmd == "at") {
        if (!cmd.parse(at.x, at.y, at.z)) {
            cmd.MarkError("Invalid values for at vector");
            return (false);
        }
        lookat_pos_set = true;
        return (true);
    } else if (cmd == "up") {
        if (!cmd.parse(up.x, up.y, up.z)) {
            cmd.MarkError("Invalid values for up vector");
            return (false);
        }
        return (true);
    } else if (cmd == "angle") {
        if (!cmd.parse(fov_angle_deg)) {
            cmd.MarkError("Invalid values for angle");
            return (false);
        }
        return (true);
    } else if (cmd == "hither") {
        cmd.MarkWarning("hither has no effect");
        return (true);
    } else if (cmd == "resolution") {
        cmd.MarkWarning("resolution has no effect");
        return (true);
    } else if (cmd == "v") {
        cmd.MarkWarning("v has no effect");
        return (true);
    } else if (cmd == "color") {
        VectorR3 ambient, diffuse, spec, trans, reflec;
        double ior;
        if (!cmd.parse(ambient.x, ambient.y, ambient.z,
                    diffuse.x, diffuse.y, diffuse.z,
                    spec.x, spec.y, spec.z,
                    trans.x, trans.y, trans.z,
                    reflec.x, reflec.y, reflec.z, ior))
        {
            cmd.MarkError("Invalid values for color");
            return (false);
        }
        cur_material->SetColorAmbient(ambient);
        cur_material->SetColorDiffuse(diffuse);
        cur_material->SetColorSpecular(spec);
        cur_material->SetColorTransmissive(trans);
        cur_material->SetColorReflective(reflec);
        cur_material->SetIndexOfRefraction(ior);
        return (true);
    } else if (cmd == "l") {
        VectorR3 center, color;
        if (cmd.parse(center.x, center.y, center.z,
                    color.x, color.y, color.z))
        {
            std::unique_ptr<Light> light(new Light());
            light->SetPosition(center);
            light->SetColor(color);
            scene.AddLight(std::move(light));
            return (true);
        } else if (cmd.parse(center.x, center.y, center.z)) {
            std::unique_ptr<Light> light(new Light());
            light->SetPosition(center);
            light->SetColor(color);
            scene.AddLight(std::move(light));
            return (true);
        } else {
            cmd.MarkError("Invalid values for light");
            return (false);
        }
    } else if (cmd == "b") {
        VectorR3 color;
        if (!cmd.parse(color.x, color.y, color.z)) {
            cmd.MarkError("Invalid values for background color");
            return (false);
        }
        scene.SetBackGroundColor(color);
        return (true);
    } else {
        // If it doesn't match anything above, run it through the config
        // commands.
        return (Load::ConfigCommand(cmd, config, /*reject_unknown = */true));
    }
}

void Load::SetCameraView(SceneDescription& scene) {
    constexpr double deg_to_rad = 180.0 / M_PI;
    double fov_angle = fov_angle_deg * deg_to_rad;
    CameraView& view = scene.GetCameraView();
    if (!lookat_pos_set) {
        AABB extents = scene.GetExtents();
        at.x = (extents.GetMaxX() + extents.GetMinX()) / 2.0;
        at.y = (extents.GetMaxY() + extents.GetMinY()) / 2.0;
        at.z = (extents.GetMaxZ() + extents.GetMinZ()) / 2.0;
    }
    if (!view_pos_set) {
        AABB extents = scene.GetExtents();
        double maxdim = std::max(extents.GetMaxX() - extents.GetMinX(),
                extents.GetMaxY() - extents.GetMinY());
        from = at;
        from.z += at.z + 1.2 * (0.5 *  maxdim / std::atan(fov_angle* 0.5 ));
    }
    view.SetLookAt(from, at, up);
    if (scene.NumLights() == 0) {
        scene.SetGlobalAmbientLight(3.5, 3.5, 3.5);
    }
    double screenDistance = view.GetScreenDistance();
    // NFF files always use aspect ratio of one
    // Resize view of theView to encompass the NFF window and more
    // But do not change the aspect ratio or pixel dimensions
    double nffScreenSize = 2.0 * screenDistance * std::tan(fov_angle * 0.5);
    double r = view.GetAspectRatio();
    if (r > 1.0) {
        view.SetScreenDimensions(r * nffScreenSize, nffScreenSize);
    } else {
        view.SetScreenDimensions(nffScreenSize, nffScreenSize / r);
    }
    view.SetClippingDistances(view.GetNearClippingDist(), -100.0);
}

void Load::ProcessPolygonVerts(SceneDescription& scene) {
    // arbitrary triangles must use increment to advance detector ids
    // detector only is used when material is sensitive
    int det_id = -1;
    if (!load_vector_source && cur_material->IsSensitive()) {
        det_id = polygon_det_id;
    }
    VectorR3 first_vert = polygon_scale * polygon_verts[0];
    VectorR3 prev_vert = polygon_scale * polygon_verts[1];
    for (size_t i = 2; i < polygon_verts.size(); ++i) {
        VectorR3 cur_vert = polygon_scale * polygon_verts[i];

        std::unique_ptr<ViewableTriangle> vt(new ViewableTriangle());
        vt->SetDetectorId(det_id);
        vt->Init(first_vert, prev_vert, cur_vert);
        vt->SetMaterial(cur_material);
        TransformWithRigid(vt.get(), matrix_stack.top());

        // FIXME: this doesn't really make any sense.  Change the source id
        // when it's added to SourceList.  Remember that vector sources can be
        // negative, which can change the id.
        vt->SetSrcId(load_vector_source ? 1:0);
        scene.AddViewable(std::move(vt));
        prev_vert = cur_vert;
    }
    polygon_verts.clear();
    no_polygon_verts = 0;
    load_polygon_lines = false;
}

/*!
 * Creates an annulus cylinder at a given point and orientation by constructing
 * an annulus at the origin, oriented along the z axis, using
 * MakeAnnulusCylinder and rotating and transforming it to using
 * RefAxisPlusTransToMap.
 */
std::vector<ViewableTriangle> Load::MakeAnnulusCylinder(
        const VectorR3 & center, const VectorR3 & axis,
        double radius_inner, double radius_outer, double width,
        int det_id, Material * material)
{
    RigidMapR3 transform = RefAxisPlusTransToMap(axis, center);
    std::vector<ViewableTriangle> pieces = MakeAnnulusCylinder(
            radius_inner, radius_outer, width);
    for (auto & triangle: pieces) {
        TransformWithRigid(&triangle, transform);
        triangle.SetDetectorId(det_id);
        triangle.SetMaterial(material);
    }
    return (pieces);
}

/*!
 * Construct a set of triangles that mimics an annulus cylinder by rotating a
 * set of four points with the appropriate width, translated in y, and rotating
 * them a given angle around the z axis.  Enforces that points to a double
 * precision are used exactly for each rotated set of points.
 *
 * Triangles are then constructed between the rotated points and the previous
 * set of four points to make four faces of points that enclose one section of
 * the cylindrical annulus.  Current two triangles are constructed per face,
 * with four faces per section, and 100 sections used for the full annulus,
 * regardless of size or shape
 *
 * \param radius_inner
 * \param radius_outer
 * \param width
 * \returns vector of 800 ViewableTriangles that create the annulus.
 */
std::vector<ViewableTriangle> Load::MakeAnnulusCylinder(
        double radius_inner, double radius_outer, double width)
{
    constexpr int no_sections = 100;
    constexpr double theta_step = M_PI * (2.0 / no_sections);
    std::vector<ViewableTriangle> pieces;
    const double pos_z = width / 2;
    const double neg_z = width / -2;

    for (int idx = 0; idx < no_sections; ++idx) {
        // rotate around the z axis to the {idx}th step of {no_sections}
        const double theta_curr = idx * theta_step;
        const double theta_next = ((idx + 1) % no_sections) * theta_step;
        const double x_curr = std::cos(theta_curr);
        const double y_curr = std::sin(theta_curr);
        const double x_next = std::cos(theta_next);
        const double y_next = std::sin(theta_next);

        const VectorR3 pos_out(radius_outer * x_curr, radius_outer * y_curr, pos_z);
        const VectorR3 neg_out(radius_outer * x_curr, radius_outer * y_curr, neg_z);
        const VectorR3 pos_in(radius_inner * x_curr, radius_inner * y_curr, pos_z);
        const VectorR3 neg_in(radius_inner * x_curr, radius_inner * y_curr, neg_z);

        const VectorR3 next_pos_out(radius_outer * x_next, radius_outer * y_next, pos_z);
        const VectorR3 next_neg_out(radius_outer * x_next, radius_outer * y_next, neg_z);
        const VectorR3 next_pos_in(radius_inner * x_next, radius_inner * y_next, pos_z);
        const VectorR3 next_neg_in(radius_inner * x_next, radius_inner * y_next, neg_z);


        // Create a all four faces, with two triangles each.
        // Top Outside
        pieces.push_back({neg_out, next_neg_out, next_pos_out});
        pieces.push_back({next_pos_out, pos_out, neg_out});
        // Positive Side
        pieces.push_back({pos_out, next_pos_out, next_pos_in});
        pieces.push_back({next_pos_in, pos_in, pos_out});
        // Bottom Inside
        pieces.push_back({pos_in, next_pos_in, next_neg_in});
        pieces.push_back({next_neg_in, neg_in, pos_in});
        // Negative Side
        pieces.push_back({neg_in, next_neg_in, next_neg_out});
        pieces.push_back({next_neg_out, neg_out, neg_in});
    }
    return (pieces);
}

void Load::DisableRayleigh(SceneDescription& scene) {
    for (size_t idx = 0; idx < scene.NumMaterials(); ++idx) {
        GammaMaterial& stats = static_cast<GammaMaterial&>(scene.GetMaterial(idx));
        stats.DisableRayleigh();
    }
}
