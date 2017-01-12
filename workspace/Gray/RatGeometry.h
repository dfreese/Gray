#ifndef RATGEOMETRY_H_
#define RATGEOMETRY_H_
#include "model.h"
#include "3ds.h"
#include "../DataStructs/Array.h"
#include "../VrMath/LinearR2.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../Graphics/TransformViewable.h"
#include "../Graphics/Material.h"
#include "../DataStructs/Stack.h"


class RatGeometry
{
public:
    void Load();

    CLoad3DS g_Load3ds;
    t3DModel g_3DModel;

};

#endif /*RATGEOMETRY_H_*/
