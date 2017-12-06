#ifndef LOAD_MATERIALS_H_
#define LOAD_MATERIALS_H_

#include <string>
#include <vector>

class SceneDescription;

class LoadMaterials
{
public:
    // TODO: allow for adding of new materials from a file
    static bool LoadPhysicsFiles(SceneDescription& theScene,
                                 const std::string & materials_filename);
    static bool LoadPhysicsJson(SceneDescription& scene,
                                const std::string & materials_filename);
private:
    static bool ParseMaterialsFile(const std::string & matfilelocation,
                                   std::vector<std::string> & material_names,
                                   std::vector<bool> & material_sensitivities);
};

#endif
