#ifndef BEAM_H
#define BEAM_H

#include <Physics/Isotope.h>
#include <Physics/BeamDecay.h>

class Beam : public Isotope
{
public:
    Beam();
    void SetBeam(const VectorR3 & axis, double angle);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    virtual void Reset();

private:
    virtual double _ExpectedNoPhotons() const;
    VectorR3 beam_axis;
    double beam_angle;
    BeamDecay beam;
};

#endif /* BEAM_H */
