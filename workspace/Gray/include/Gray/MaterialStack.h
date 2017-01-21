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

#include <Gray/GammaMaterial.h>

#include <VrMath/LinearR3.h>
#include <DataStructs/Stack.h>

class MaterialStack
{
public:
    MaterialStack();
    void PushMaterial(GammaMaterial * mat);
    GammaMaterial * PopMaterial();
    GammaMaterial * curMaterial();
    void ResetMaterial();
    int NumMaterials() const;
    void SetDefault(GammaMaterial * mat);

private:
    Stack<GammaMaterial*> MatStack;
    GammaMaterial * defaultMaterial;
};

#endif
