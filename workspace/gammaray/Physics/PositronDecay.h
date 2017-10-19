#ifndef GAMMAPOSITRONDECAY_H
#define GAMMAPOSITRONDECAY_H

#include <Physics/NuclearDecay.h>

class PositronDecay : public NuclearDecay
{
public:
    PositronDecay() = default;
    void Decay(int photon_number, double time, int src_id,
               const VectorR3 & position) override;
};

#endif
