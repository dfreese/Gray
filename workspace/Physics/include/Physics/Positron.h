#ifndef GAMMAPOSITRON_H
#define GAMMAPOSITRON_H

#include <Physics/Isotope.h>
#include <Physics/PositronDecay.h>

class Positron: public Isotope
{
public:
    Positron(double acolinearity_deg_fwhm, double half_life);
    Positron(double acolinearity_deg_fwhm, double half_life,
             double positron_emis_prob);
    Positron(double acolinearity_deg_fwhm, double half_life,
             double gamma_decay_energy_mev, double positron_emis_prob);
    virtual void Reset();
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    static void PositronRange(VectorR3 & p, double positronC,
                              double positronK1, double positronK2,
                              double positronMaxRange);
    static void PositronRange(VectorR3 & p, double positronFWHM,
                              double positronMaxRange);

    static const double default_acolinearity;

protected:
    bool use_positron_dbexp;
    bool use_positron_gauss;
    double positronFWHM; // expressed in meters
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;

private:
    PositronDecay p;
};

#endif /* POSITRON_H */
