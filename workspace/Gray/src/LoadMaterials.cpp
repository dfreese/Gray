#include <Gray/LoadMaterials.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <Gray/GammaMaterial.h>
#include <Graphics/SceneDescription.h>

bool LoadMaterials::LoadPhysicsFiles(SceneDescription& theScene)
{
    char * pPath = getenv ("GRAY_INCLUDE");
    if (pPath==NULL) {
        printf("No GRAY_INCLUDE variable set !\n");
        printf("Execute export GRAY_INCLUDE=yourpath\n");
        return(false);
    }
    string include_location(pPath);
    string matfilelocation = include_location + "/Gray_Materials.txt";
    ifstream matfile(matfilelocation.c_str());

    if (!matfile.is_open()) {
        cout << matfilelocation << " not found. Using hard coded material table " << endl;
        return(false);
    }
    
    vector<string> material_names;
    vector<bool> material_sensitivities;
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
        int matnumber;
        bool matsensitive;
        
        stringstream line_ss;
        line_ss << matstring;
        line_ss >> matname;
        line_ss >> matchemform;
        line_ss >> matdens;
        line_ss >> matsensitive;
        line_ss >> matnumber;
        material_names.push_back(matname);
        material_sensitivities.push_back(matsensitive);
    }
    cout << material_names.size() << " Materials Found " <<endl;
    matfile.close();
    return(LoadMaterials::Load(material_names, material_sensitivities, theScene));
}


bool LoadMaterials::Load(
        const std::vector<std::string> material_names,
        const std::vector<bool> material_sensitivities,
        SceneDescription& theScene)
{
    bool parseOk = true;
    int numMaterialLoaded = 0;
    for (size_t i = 0; i < material_names.size(); i++) {
        GammaMaterial * mat = new GammaMaterial();
        theScene.AddMaterial(mat);
        std::string material_filename = material_names[i] + ".dat";
        dynamic_cast<Material*>(mat)->SetName(material_names[i]);
        mat->SetFileName(material_filename);
        mat->SetMaterialType( i) ;
        if (!mat->Load()) {
            return false;
        }
        mat->log_material = material_sensitivities[i];
        if (i == 0) { // First material is default material
            mat->DisableInteractions();
        }
        numMaterialLoaded++;
    }
    return parseOk;
}