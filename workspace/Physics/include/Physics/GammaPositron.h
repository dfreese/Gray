#ifndef GAMMAPOSITRON_H
#define GAMMAPOSITRON_H

#include <Physics/Isotope.h>
#include <Physics/GammaPositronDecay.h>

class GammaPositron: public Isotope
{
public:
    GammaPositron(double acolinearity_deg_fwhm, double half_life);
    GammaPositron(double acolinearity_deg_fwhm,
                  double half_life,
                  double positron_emis_prob);
    GammaPositron(double acolinearity_deg_fwhm,
                  double half_life,
                  double gamma_decay_energy_mev,
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
    GammaPositronDecay gp;
};

#endif /* POSITRON_H */
