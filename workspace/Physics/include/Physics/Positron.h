#ifndef POSITRON_H
#define POSITRON_H

#include <limits>
#include <Physics/Isotope.h>
#include <Physics/PositronDecay.h>

class Positron: public Isotope
{
public:
    Positron(double acolinearity_deg_fwhm,
             double half_life,
             double positron_emis_prob);
    virtual void Reset();
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);

protected:
    bool use_positron_dbexp;
    bool use_positron_gauss;
    double positronFWHM; // expressed in meters
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;
    PositronDecay p;
};

#endif /* POSITRON_H */
