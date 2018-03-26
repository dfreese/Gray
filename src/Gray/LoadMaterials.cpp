#include "Gray/Gray/LoadMaterials.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "Gray/Gray/GammaMaterial.h"
#include "Gray/Graphics/SceneDescription.h"
#include "Gray/Physics/GammaStats.h"
#include "Gray/json/json.h"

std::vector<double> LoadMaterials::VectorizeArray(const Json::Value & array) {
    if (!array.isArray()) {
        return (std::vector<double>());
    }
    std::vector<double> values(array.size());
    // json arrays are indexed by ints
    for (int ii = 0; ii < static_cast<int>(array.size()); ++ii) {
        values[ii] = array[ii].asDouble();
    }
    return (values);
}

bool LoadMaterials::LoadMaterialJson(SceneDescription& scene,
                                     const std::string & mat_name,
                                     const Json::Value & mat_info)
{
    auto required_vals = {
        "density",
        "index",
        "energy",
        "form_factor",
        "matten_comp",
        "matten_phot",
        "matten_rayl",
        "scattering_func",
        "sensitive",
        "x",
    };
    for (auto val : required_vals) {
        if (mat_info[val].isNull()) {
            std::cerr << val << "does not exist in " << mat_name << "\n";
            return (false);
        }
    }
    double density = mat_info["density"].asDouble();
    int index = mat_info["index"].asInt();
    bool sensitive = mat_info["sensitive"].asBool();
    std::vector<double> energy = VectorizeArray(mat_info["energy"]);
    std::vector<double> matten_comp = VectorizeArray(mat_info["matten_comp"]);
    std::vector<double> matten_phot = VectorizeArray(mat_info["matten_phot"]);
    std::vector<double> matten_rayl = VectorizeArray(mat_info["matten_rayl"]);
    std::vector<double> x = VectorizeArray(mat_info["x"]);
    std::vector<double> form_factor = VectorizeArray(mat_info["form_factor"]);
    std::vector<double> scattering_func =
            VectorizeArray(mat_info["scattering_func"]);
    
    GammaStats stats(density, energy, matten_comp, matten_phot, matten_rayl,
             x, form_factor, scattering_func);
    std::unique_ptr<GammaMaterial> mat(new GammaMaterial(
            index, mat_name, sensitive, true, std::move(stats)));
    scene.AddMaterial(std::move(mat));
    return (true);
}

bool LoadMaterials::LoadPhysicsJson(SceneDescription& scene,
                                    const std::string & physics_filename)
{
    Json::Value root;
    Json::Reader reader;
    std::ifstream input(physics_filename);
    std::stringstream buffer;
    buffer << input.rdbuf();

    bool status = reader.parse(buffer.str(), root,
                               /*collect_comments=*/false);
    if (!status) {
        std::cerr << "Reading of Physics File, \""
                  << physics_filename << "\" failed\n"
                  << reader.getFormattedErrorMessages() << "\n";
        return (false);
    }

    Json::Value materials = root["materials"];
    if (materials.isNull()) {
        std::cerr << "Reading of Physics File, \""
                  << physics_filename << "\" failed\n"
                  << "Json file does not have \"materials\" member\n";
        return (false);
    }

    for (const std::string & mat_name : materials.getMemberNames ()) {
        bool status = LoadMaterialJson(scene, mat_name, materials[mat_name]);
        if (!status) {
            std::cerr << "Reading of Physics File, \""
                      << physics_filename << "\" failed\n"
                      << "Unable to load material, \"" << mat_name << "\"\n";
            return (false);
        }
    }

    // Create a dummy material through which the photons can propogate without
    // interacting.  This just makes the logic easier to propogate photons
    // through space where there is no material.  If the user wants a
    // background material, then they would need to create a box of that
    // material.
    // Perhaps look at allowing the default to be specified.
    const std::string def_mat_name = "dummy_default_world_material";

    GammaStats stats(0.0,
            std::vector<double>(1, 0.0),
            std::vector<double>(1, 0.0),
            std::vector<double>(1, 0.0),
            std::vector<double>(1, 0.0),
            std::vector<double>(1, 0.0),
            std::vector<double>(1, 0.0),
            std::vector<double>(1, 0.0));
    std::unique_ptr<GammaMaterial> default_mat(new GammaMaterial(
            scene.NumMaterials(), def_mat_name, false, false,
            std::move(stats)));
    scene.AddMaterial(std::move(default_mat));
    scene.SetDefaultMaterial(def_mat_name);
    return (true);
}
