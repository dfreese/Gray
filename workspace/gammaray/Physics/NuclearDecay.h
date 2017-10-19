#ifndef NUCLEARDECAY_H
#define NUCLEARDECAY_H

#include <stack>
#include <Physics/Photon.h>
#include <VrMath/LinearR3.h>

class NuclearDecay
{
public:
    NuclearDecay();
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position) = 0;
    virtual void Reset();
    Photon NextPhoton();
    bool IsEmpty() const;
    double GetEnergy() const;
    int GetDecayNumber() const;
    int GetSourceId() const;
    VectorR3 GetPosition() const;
    double GetTime() const;

protected:
    void AddPhoton(Photon && p);
    
    double energy;
    int decay_number;
    int src_id;
    VectorR3 position;
    double time;

private:
    std::stack<Photon> daughter;
};

#endif /* NUCLEARDECAY_H */
