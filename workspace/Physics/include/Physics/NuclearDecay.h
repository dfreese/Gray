#ifndef NUCLEARDECAY_H
#define NUCLEARDECAY_H

#include <VrMath/LinearR3.h>
#include <Physics/Photon.h>
#include <stack>

class NuclearDecay
{
public:
    NuclearDecay();
    void SetTime(const double t);
    double Random();
    void SetBeam(const VectorR3 &axis, const double angle);
    int GetSourceNum(void) const;
    virtual void SetPosition(const VectorR3 & p) = 0;
    virtual void Decay(int photon_number) = 0;
    virtual void Reset() = 0;
    Photon * NextPhoton();
    bool IsEmpty() const;
    void AddPhoton(Photon * p);

public:
    double time;
    bool beamDecay;
    double energy;
    VectorR3 pos;
    int decay_number;
    int source_num;

protected:
    double beam_angle;
    VectorR3 beam_axis;
    std::stack<Photon *> daughter;
};

#endif /* NUCLEARDECAY_H */
