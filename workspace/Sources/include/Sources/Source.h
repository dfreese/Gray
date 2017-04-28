/*
 * Source.h
 */

#ifndef SOURCE_H
#define SOURCE_H

#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Physics/Isotope.h>
#include <Physics/Photon.h>

class GammaMaterial;

class Source
{
public:
    Source() :
        isotope(NULL),
        activity(0),
        material(NULL),
        negative(false),
        source_num(0),
        position(0, 0, 0)
    {
    }

    Source(const VectorR3 & pos, double act) :
        isotope(NULL),
        activity(act),
        material(NULL),
        negative(false),
        source_num(0),
        position(pos)
    {
        if (act < 0.0) {
            negative = true;
        } else {
            negative = false;
        }
    }
    virtual ~Source() {}

    double GetActivity() const {
        return activity;
    }

    void SetIsotope(Isotope * i)
    {
        isotope = i;
    }

    void SetMaterial(GammaMaterial * mat) {
        material = mat;
    }

    void SetPosition(const VectorR3 & pos)
    {
        position = pos;
    }

    void SetSourceNum(int i)
    {
        source_num = i;
    }

    GammaMaterial * GetMaterial() {
        return material;
    }

    GammaMaterial * const GetMaterial() const {
        return material;
    }

    bool isNegative() const {
        return negative;
    }

    Isotope * GetIsotope()
    {
        return isotope;
    }

    void Reset() {
        if (isotope == NULL) {
            return;
        }
        isotope->Reset();
    }

    virtual bool Inside(const VectorR3 &pos) const = 0;
    virtual VectorR3 Decay(int photon_number, double time) = 0;

protected:
    Isotope * isotope;
    double activity;
    GammaMaterial * material;
    bool negative;
    int source_num;
    VectorR3 position;
};

#endif // SOURCE_H
