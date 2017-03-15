#ifndef GAMMADECAY_H
#define GAMMADECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class GammaDecay : public NuclearDecay
{
public:
    GammaDecay();
    virtual void SetPosition(const VectorR3 & p);
    virtual void Decay(unsigned int photon_number);
    virtual void Reset()
    {
        gamma.Reset();
        gamma.color = Photon::P_YELLOW;
    }
    virtual std::ostream & print_on(std::ostream & os) const;
    void SetEnergy(double e)
    {
        energy = e;
    };

public:
    Photon gamma;
    double energy;
};

#endif /* GAMMADECAY_H */
