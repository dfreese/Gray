/*
 * Source.h
 */

#ifndef SOURCE_H
#define SOURCE_H

#include <memory>
#include <stack>
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
        isotope(nullptr),
        activity(0),
        negative(false),
        source_num(0),
        position(0, 0, 0)
    {
    }

    Source(const VectorR3 & pos, double act) :
        isotope(nullptr),
        activity(act),
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

    virtual void SetIsotope(std::unique_ptr<Isotope> i)
    {
        isotope = std::move(i);
    }

    void SetPosition(const VectorR3 & pos)
    {
        position = pos;
    }

    VectorR3 GetPosition() const {
        return (position);
    }

    void SetSourceNum(int i)
    {
        source_num = i;
    }

    void SetMaterialStack(std::stack<GammaMaterial const *> material_stack) {
        this->material_stack = material_stack;
    }

    const std::stack<GammaMaterial const *> & GetMaterialStack() const {
        return(material_stack);
    }

    bool isNegative() const {
        return negative;
    }

    Isotope * GetIsotope()
    {
        return isotope.get();
    }

    void Reset() {
        isotope->Reset();
    }

    virtual bool Inside(const VectorR3 &pos) const = 0;
    virtual VectorR3 Decay(int photon_number, double time) = 0;

protected:
    std::unique_ptr<Isotope> isotope;
    double activity;
    bool negative;
    int source_num;
    VectorR3 position;

private:
    std::stack<GammaMaterial const *> material_stack;
};

#endif // SOURCE_H
