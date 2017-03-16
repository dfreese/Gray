#ifndef GAMMADECAY_H
#define GAMMADECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class GammaDecay : public NuclearDecay
{
public:
    GammaDecay();
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    virtual void Reset();
    void SetEnergy(double e);

private:
    double energy;
    Photon gamma;
};

#endif /* GAMMADECAY_H */
