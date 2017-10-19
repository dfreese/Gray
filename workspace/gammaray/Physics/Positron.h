#ifndef GAMMAPOSITRON_H
#define GAMMAPOSITRON_H

#include <Physics/Isotope.h>
#include <Physics/PositronDecay.h>

class Positron: public Isotope
{
public:
    Positron();
    Positron(double acolinearity_deg_fwhm, double half_life);
    Positron(double acolinearity_deg_fwhm, double half_life,
             double positron_emis_prob);
    Positron(double acolinearity_deg_fwhm, double half_life,
             double gamma_decay_energy_mev, double positron_emis_prob);
    virtual void Reset();
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    void SetPositronRange(double c, double k1, double k2, double max);
    void SetPositronRange(double fwhm, double max);

private:
    // TODO: evaluate creating this versus having it as a part of the class.
    // This makes it impossible to parallelize the raytracing of the photons.
    PositronDecay p;
    bool use_positron_dbexp;
    bool use_positron_gauss;
    double positronFWHM; // expressed in meters
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;
    static VectorR3 PositronRangeLevin(const VectorR3 & p, double positronC,
                                       double positronK1, double positronK2,
                                       double positronMaxRange);
    static VectorR3 PositronRangeGauss(const VectorR3 & p, double positronFWHM,
                                       double positronMaxRange);
    virtual double _ExpectedNoPhotons() const;

    double acolinearity;
    double gamma_decay_energy;
    double positron_emission_prob;
    bool emit_gamma;
};

#endif /* POSITRON_H */
