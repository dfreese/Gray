#ifndef LOAD_MATERIALS_H_
#define LOAD_MATERIALS_H_

class SceneDescription;

class LoadMaterials
{
public:
    // TODO: allow for adding of new materials from a file
    static bool LoadPhysicsFiles(SceneDescription& theScene);
    static bool Load(int num, SceneDescription& theScene); // load default materials
};

#endif
