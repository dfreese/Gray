#ifndef POSITRON_H
#define POSITRON_H

#include <Physics/Isotope.h>
#include <Physics/PositronDecay.h>

class Positron: public Isotope
{
public:
    Positron();

protected:
    bool positronRange;
    bool positronRangeGaussian;
    bool positronRangeCusp;
    double positronFWHM; // expressed in meters
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;
    PositronDecay p;
};

#endif /* POSITRON_H */
