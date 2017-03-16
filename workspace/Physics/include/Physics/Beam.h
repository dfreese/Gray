#ifndef BEAM_H
#define BEAM_H

#include <Physics/Positron.h>

class Beam : public Positron
{
public:
    Beam();
    void SetBeam(const VectorR3 & axis, double angle);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    virtual void Reset();
private:
    VectorR3 beam_axis;
    double beam_angle;
};

#endif /* BEAM_H */
