#include <Gray/LoadMaterials.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <Gray/GammaMaterial.h>
#include <Graphics/SceneDescription.h>

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
        GammaMaterial * mat = new GammaMaterial();
        std::string material_filename = include_location + "/" +
                                        material_names[i] + ".dat";
        dynamic_cast<Material*>(mat)->SetName(material_names[i]);
        mat->SetFileName(material_filename);
        mat->SetMaterialType(static_cast<int>(i));
        if (!mat->Load()) {
            return(false);
        }
        mat->log_material = material_sensitivities[i];
        if (i == 0) { // First material is default material
            mat->DisableInteractions();
        }
        numMaterialLoaded++;
        theScene.AddMaterial(mat);
    }
    return(true);
}
