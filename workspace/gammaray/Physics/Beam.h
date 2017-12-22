#ifndef BEAM_H
#define BEAM_H

#include <Physics/Isotope.h>

class Beam : public Isotope
{
public:
    Beam();
    void SetBeam(const VectorR3 & axis, double angle, double energy);
    NuclearDecay Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position) override;
    double ExpectedNoPhotons() const override;

private:
    VectorR3 beam_axis;
    double beam_angle;
    double beam_energy;
};

#endif /* BEAM_H */
