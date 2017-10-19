#ifndef BEAMDECAY_H
#define BEAMDECAY_H

#include <Physics/NuclearDecay.h>
#include <VrMath/LinearR3.h>

class BeamDecay : public NuclearDecay
{
public:
    BeamDecay();
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
};

#endif /* BEAMDECAY_H */
