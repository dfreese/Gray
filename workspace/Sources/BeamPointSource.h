#ifndef BEAMPOINTSOURCE_H_
#define BEAMPOINTSOURCE_H_

#include "Source.h"

class BeamPointSource : public Source
{
public:
    BeamPointSource();
    BeamPointSource(const VectorR3 &p, const VectorR3 &a, double angle, double act);
    // gets a beam positron instead of a regular one
    void virtual Decay(unsigned int photon_number);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    VectorR3 beam_axis;
    double beam_angle;
};

#endif /*BEAMPOINTSOURCE_H_*/
