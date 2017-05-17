#ifndef NUCLEARDECAY_H
#define NUCLEARDECAY_H

#include <VrMath/LinearR3.h>
#include <stack>

class Photon;

class NuclearDecay
{
public:
    NuclearDecay();
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position) = 0;
    virtual void Reset();
    Photon * NextPhoton();
    bool IsEmpty() const;
    void AddPhoton(Photon * p);
    double GetEnergy() const;
    int GetDecayNumber() const;
    int GetSourceId() const;
    VectorR3 GetPosition() const;
    double GetTime() const;

protected:
    std::stack<Photon *> daughter;
    double energy;
    int decay_number;
    int src_id;
    VectorR3 position;
    double time;
};

#endif /* NUCLEARDECAY_H */