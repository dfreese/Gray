#ifndef LOAD_MATERIALS_H_
#define LOAD_MATERIALS_H_

#include <string>
#include <vector>

class SceneDescription;

class LoadMaterials
{
public:
    // TODO: allow for adding of new materials from a file
    static bool LoadPhysicsFiles(SceneDescription& theScene);
    static bool Load(const std::vector<std::string> material_names,
                     const std::vector<bool> material_sensitivities,
                     SceneDescription& theScene);
};

#endif