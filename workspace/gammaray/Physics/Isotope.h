#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <limits>
#include <memory>
#include <VrMath/LinearR3.h>
#include <Physics/NuclearDecay.h>

class Isotope
{
public:
    Isotope() = default;
    Isotope(double half_life_s);
    virtual ~Isotope() = default;
    virtual std::unique_ptr<Isotope> Clone() = 0;
    virtual NuclearDecay Decay(int photon_number, double time, int src_id,
                               const VectorR3 & position) const = 0;
    double GetHalfLife() const;
    double FractionRemaining(double time) const;
    double FractionIntegral(double start, double time) const;
    virtual double ExpectedNoPhotons() const = 0;

private:
    double half_life = std::numeric_limits<double>::infinity();
};

#endif /* ISOTOPE_H */
