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
    Isotope(double half_life_s);
    virtual ~Isotope() {};
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position) = 0;
    virtual void Reset();
    NuclearDecay * NextNuclearDecay();
    bool IsEmpty() const;
    double FractionRemaining(double time) const;

protected:
    void AddNuclearDecay(NuclearDecay * nd);

private:
    double half_life;
    std::stack<NuclearDecay*> daughter;
};

#endif /* ISOTOPE_H */
