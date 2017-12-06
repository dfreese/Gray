#include <Gray/LoadMaterials.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <Gray/GammaMaterial.h>
#include <Graphics/SceneDescription.h>
#include <json/json.h>

bool LoadMaterials::ParseMaterialsFile(
        const std::string & matfilelocation,
        std::vector<std::string> & material_names,
        std::vector<bool> & material_sensitivities)
{
    ifstream matfile(matfilelocation.c_str());

    if (!matfile.is_open()) {
        cout << matfilelocation << " not found." << endl;
        return(false);
    }

    string matstring;
    while (getline(matfile, matstring)) {
        if (matstring.empty()) {
            continue;
        } else if (matstring[0] == '#') {
            continue;
        }
        string matname;
        string matchemform;
        float matdens;
        bool matsensitive;

        stringstream line_ss(matstring);
        line_ss >> matname;
        line_ss >> matchemform;
        line_ss >> matdens;
        line_ss >> matsensitive;
        material_names.push_back(matname);
        material_sensitivities.push_back(matsensitive);
    }
    matfile.close();
    return(true);
}


bool LoadMaterials::LoadPhysicsFiles(SceneDescription& theScene,
                                     const std::string & materials_filename)
{
    // Find the directory in the filename.
    // FIXME: Not generally portable.
    std::string include_location = "";
    size_t dir_pos = materials_filename.find_last_of('/');
    if (dir_pos != std::string::npos) {
        // Include everything, excluding slash
        include_location = materials_filename.substr(0, dir_pos);
    }

    vector<string> material_names;
    vector<bool> material_sensitivities;
    if (!ParseMaterialsFile(materials_filename,
                            material_names,
                            material_sensitivities))
    {
        return(false);
    }

    int numMaterialLoaded = 0;
    for (size_t i = 0; i < material_names.size(); i++) {
        std::unique_ptr<GammaMaterial> mat(new GammaMaterial());
        std::string material_filename = include_location + "/" +
                                        material_names[i] + ".dat";
        static_cast<Material*>(mat.get())->SetName(material_names[i]);
        mat->SetFileName(material_filename);
        mat->SetMaterialType(static_cast<int>(i));
        if (!mat->Load()) {
            return(false);
        }
        mat->SetLogMaterial(material_sensitivities[i]);
        if (i == 0) { // First material is default material
            mat->DisableInteractions();
        }
        numMaterialLoaded++;
        theScene.AddMaterial(std::move(mat));
    }
    return(true);
}

namespace {
std::vector<double> VectorizeArray(const Json::Value & array) {
    if (!array.isArray()) {
        return (std::vector<double>());
    }
    std::vector<double> values(array.size());
    for (int ii = 0; ii < array.size(); ++ii) {
        values[ii] = array[ii].asDouble();
    }
    return (values);
}

bool LoadMaterialJson(SceneDescription& scene,
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

    std::unique_ptr<GammaMaterial> mat(new GammaMaterial(
            mat_name, index, density, sensitive, energy, matten_comp,
            matten_phot, matten_rayl, x, form_factor, scattering_func));
    scene.AddMaterial(std::move(mat));
    return (true);
}
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
    Json::Value def_mat = root["defaults"]["world_material"];
    if (def_mat.isNull()) {
        std::cerr << "Reading of Physics File, \""
                  << physics_filename << "\" failed\n"
                  << "Unable to load default material" << "\"\n";
        return (false);
    }
    const std::string def_mat_name = def_mat.asString();

    if (!scene.HasMaterial(def_mat_name)) {
        std::cerr << "Default material, \""
                  << def_mat_name << "\" is not valid\n";
        return (false);
    }

    scene.SetDefaultMaterial(def_mat_name);
    // TODO: make blank world material that doesn't take interactions, so
    // we don't just disable a material out from underneath the user....
    static_cast<GammaMaterial&>(scene.GetDefaultMaterial()).DisableInteractions();

    return (true);
}
