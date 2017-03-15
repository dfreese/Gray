#ifndef GAMMADECAY_H
#define GAMMADECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class GammaDecay : public NuclearDecay
{
public:
    GammaDecay();
    virtual void SetPosition(const VectorR3 & p);
    virtual void Decay(int photon_number);
    virtual void Reset();
    virtual std::ostream & print_on(std::ostream & os) const;
    void SetEnergy(double e);

private:
    double energy;
    Photon gamma;
};

#endif /* GAMMADECAY_H */
