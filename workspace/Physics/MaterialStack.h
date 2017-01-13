/*
 * SimpleNurbs.h
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 */

#ifndef CSE167_MATERIALSTACK_H
#define CSE167_MATERIALSTACK_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>

#include "../Graphics/Material.h"
#include "../Graphics/VisiblePoint.h"
#include "../VrMath/LinearR3.h"
#include "../DataStructs/Stack.h"

#define MAX_MATERIALS 10
#define ERROR_MATERIAL (MaterialBase*)-2
#define ERROR_MATERIAL_TRACE_DEPTH (MaterialBase*)-3

class MaterialStack
{
public:
    MaterialStack();
    void PushMaterial(MaterialBase * mat);
    MaterialBase * PopMaterial();
    MaterialBase * curMaterial();
    void ResetMaterial();
    int NumMaterials() const
    {
        return MatStack.Size();
    }
    void SetDefault(MaterialBase * mat);

private:
    Stack<MaterialBase*> MatStack;
    MaterialBase * defaultMaterial;
    MaterialBase * errorMaterial;
};

#endif