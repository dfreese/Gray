#ifndef LOAD_MATERIALS_H_
#define LOAD_MATERIALS_H_

#include "../Gray/SceneDescription.h"
#include "../Graphics/MaterialBase.h"

#include "GammaStats.h"

bool LoadPhysicsFiles( SceneDescription& theScene );

class LoadMaterials
{
public:
    // TODO: allow for adding of new materials from a file
    bool Load( int num, SceneDescription& theScene ); // load default materials
private:
    int numMaterialLoaded;
};

#endif
