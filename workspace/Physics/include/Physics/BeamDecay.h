#ifndef BEAMDECAY_H
#define BEAMDECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>
#include <VrMath/LinearR3.h>

class BeamDecay : public NuclearDecay
{
public:
    BeamDecay();
    void SetBeam(const VectorR3 &axis, const double angle);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);

private:
    Photon blue;
    Photon red;
    double angle;
    VectorR3 axis;
};

#endif /* BEAMDECAY_H */
