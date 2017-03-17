#ifndef POSITRON_H
#define POSITRON_H

#include <Physics/Isotope.h>
#include <Physics/PositronDecay.h>

class Positron: public Isotope
{
public:
    Positron(double acolinearity_deg_fwhm);
    virtual void Reset();

protected:
    double positronFWHM; // expressed in meters
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;
    PositronDecay p;
};

#endif /* POSITRON_H */
