#ifndef BEAM_H
#define BEAM_H

#include <Physics/Positron.h>

class Beam : public Positron
{
public:
    Beam();
    void SetBeam(const VectorR3 & axis, double angle);
    virtual void Decay(unsigned int photon_number, double time);
    virtual void Reset();
    virtual std::ostream & print_on(std::ostream & os) const;
private:
    VectorR3 beam_axis;
    double beam_angle;
};

#endif /* BEAM_H */
