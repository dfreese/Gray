#ifndef GAMMADECAY_H
#define GAMMADECAY_H

#include "NuclearDecay.h"
#include "Photon.h"

class GammaDecay : public NuclearDecay
{
public:
    GammaDecay();
    virtual void SetPosition(const VectorR3 & p);
    virtual void Decay(unsigned int photon_number);
    virtual void Reset()
    {
        gamma.Reset();
        gamma.color = P_YELLOW;
    }
    virtual ostream & print_on( ostream& os ) const;
    virtual DECAY_TYPE GetType() const
    {
        return GAMMA;
    };
    void SetEnergy(double e)
    {
        energy = e;
    };

public:
    Photon gamma;
    double energy;
};

#endif /* GAMMADECAY_H */
