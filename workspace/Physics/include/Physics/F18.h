#ifndef F18_H
#define F18_H

#include <Physics/Positron.h>

class F18 : public Positron
{
public:
    F18(double acolinearity_deg_fwhm);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
protected:
};

#endif /* F18_H */
