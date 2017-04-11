#ifndef GAMMAPOSITRON_H
#define GAMMAPOSITRON_H

#include <Physics/Isotope.h>
#include <Physics/GammaPositronDecay.h>

class GammaPositron: public Isotope
{
public:
    GammaPositron(double acolinearity_deg_fwhm,
                  double half_life,
                  double gamma_decay_energy_mev,
                  double positron_emis_prob);
    virtual void Reset();

protected:
    double positronFWHM; // expressed in meters
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;
    GammaPositronDecay gp;
};

#endif /* POSITRON_H */
