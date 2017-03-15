#ifndef NUCLEARDECAY_H
#define NUCLEARDECAY_H

#include <VrMath/LinearR3.h>

class MaterialBase;

class NuclearDecay
{
public:
    NuclearDecay();
    void SetTime(const double t)
    {
        time = t;
    };
    double Random();
    friend std::ostream & operator<< (std::ostream & os, const NuclearDecay & n);
    MaterialBase * GetMaterial();
    void SetMaterial(MaterialBase * mat);
    void SetBeam(const VectorR3 &axis, const double angle);
    int GetSourceNum(void)
    {
        return source_num;
    };
    virtual void SetPosition(const VectorR3 & p) = 0;
    virtual void Decay(unsigned int photon_number) = 0;
    virtual void Reset() = 0;
    virtual std::ostream & print_on(std::ostream &) const = 0;

public:
    double time;
    bool beamDecay;
    double energy;
    VectorR3 pos;
    unsigned int decay_number;
    int source_num;

protected:
    double beam_angle;
    void BeamCone();

    VectorR3 beam_axis;

    MaterialBase * material;

    inline friend std::ostream & operator<< (std::ostream &os,
                                             const NuclearDecay & n)
    {
        return n.print_on(os);
    }
};

inline MaterialBase * NuclearDecay::GetMaterial()
{
    return material;
}

inline void NuclearDecay::SetMaterial(MaterialBase * mat)
{
    material = mat;
}

#endif /* NUCLEARDECAY_H */
