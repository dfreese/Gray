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

#ifndef CSE167_SOURCE_H
#define CSE167_SOURCE_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Physics/Isotope.h>
#include <Physics/Photon.h>

using namespace std;
class GammaMaterial;
class IntersectKdTree;

class Source
{
public:
    Source() {
        material = NULL;
        activity = 0.0;
        negative = false;
        isotope = NULL;
        source_num = 0;
        kd_tree = NULL;
    }
    virtual ~Source() {};
    double GetActivity();
    virtual void SetIsotope(Isotope * i)
    {
        isotope = i;
    };
    void SetActivity(double act);
    void SetMaterial(GammaMaterial * mat);
    void SetPosition(const VectorR3 & pos)
    {
        position = pos;
    }
    void SetSourceNum(int i)
    {
        source_num = i;
    }
    GammaMaterial * GetMaterial();
    bool isNegative();
    Isotope * GetIsotope()
    {
        return isotope;
    }
    void Reset();
    void SetKdTree(IntersectKdTree & tree) {
        kd_tree = &tree;
    }

    virtual bool Inside(const VectorR3 &pos) const = 0;
    virtual VectorR3 Decay(int photon_number, double time) = 0;

protected:
    Isotope * isotope;
    double activity;
    GammaMaterial * material;
    bool negative;
    int source_num;
    IntersectKdTree * kd_tree;
    VectorR3 position;
};

inline bool Source::isNegative()
{
    return negative;
}

inline double Source::GetActivity()
{
    return activity;
}

inline void Source::SetActivity(double act)
{
    activity = act;
    if (act < 0.0) {
        negative = true;
    } else {
        negative = false;
    }
}

inline void Source::SetMaterial(GammaMaterial * mat)
{
    material = mat;
}

inline GammaMaterial * Source::GetMaterial()
{
    return material;
}

inline void Source::Reset()
{
    //cout << "Reset\n";
    if (isotope == NULL) {
        return;
    }
    isotope->Reset();
}

#endif
