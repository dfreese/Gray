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
    Isotope() = default;
    Isotope(double half_life_s);
    virtual ~Isotope() = default;
    virtual NuclearDecay Decay(int photon_number, double time, int src_id,
                               const VectorR3 & position) = 0;
    double GetHalfLife() const;
    double FractionRemaining(double time) const;
    virtual double ExpectedNoPhotons() const = 0;

private:
    double half_life = std::numeric_limits<double>::infinity();
};

#endif /* ISOTOPE_H */
