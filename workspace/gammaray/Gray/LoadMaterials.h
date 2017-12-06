#ifndef LOAD_MATERIALS_H_
#define LOAD_MATERIALS_H_

#include <string>
#include <vector>
#include <json/json.h>

class SceneDescription;

namespace LoadMaterials
{
std::vector<double> VectorizeArray(const Json::Value & array);
bool LoadMaterialJson(SceneDescription& scene,
                      const std::string & mat_name,
                      const Json::Value & mat_info);
bool LoadPhysicsJson(SceneDescription& scene,
                     const std::string & materials_filename);
};

#endif
