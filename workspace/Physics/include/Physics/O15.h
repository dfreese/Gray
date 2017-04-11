#ifndef O15_H
#define O15_H

#include <Physics/Positron.h>

class O15 : public Positron
{
public:
    O15(double acolinearity_deg_fwhm);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
protected:
};

#endif // O15_H
