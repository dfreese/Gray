#ifndef IN110_H
#define IN110_H

#include <Physics/GammaPositron.h>

class IN110 : public GammaPositron
{
public:
    IN110(double acolinearity_deg_fwhm);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
};

#endif /* IN110_H */
