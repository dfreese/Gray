#ifndef POSITRON_H
#define POSITRON_H

#include <Physics/Isotope.h>
#include <Physics/PositronDecay.h>

class Positron: public Isotope
{
public:
    Positron();
    void SetPositronRange(double c, double e1, double e2, double max_range);
    void SetPositronRange(double gauss, double max_range);
    PositronDecay * GetPositronDecay();

protected:
    bool positronRange;
    bool positronRangeGaussian;
    bool positronRangeCusp;
    double positronFWHM; // expressed in meters
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;
    void PositronRange(PositronDecay &p);
    PositronDecay p;
};

#endif /* POSITRON_H */
