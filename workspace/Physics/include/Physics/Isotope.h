#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <map>
#include <stack>
#include <string>
#include <VrMath/LinearR3.h>
#include <Physics/NuclearDecay.h>

class Isotope
{
public:
    Isotope();
    virtual ~Isotope() {};
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position) = 0;
    virtual void Reset() = 0;
    NuclearDecay * NextNuclearDecay();
    bool IsEmpty() const;
protected:
    double half_life;
    void AddNuclearDecay(NuclearDecay * nd);
    std::stack<NuclearDecay*> daughter;
};

#endif /* ISOTOPE_H */
