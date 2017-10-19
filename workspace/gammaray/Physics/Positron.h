#ifndef GAMMAPOSITRON_H
#define GAMMAPOSITRON_H

#include <Physics/Isotope.h>

class Positron: public Isotope
{
public:
    Positron() = default;
    Positron(double acolinearity_deg_fwhm, double half_life,
             double gamma_decay_energy_mev, double positron_emis_prob);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    void SetPositronRange(double c, double k1, double k2, double max);
    void SetPositronRange(double fwhm_mm, double max_mm);

private:
    bool use_positron_dbexp = false;
    bool use_positron_gauss = false;
    double positron_max_range_cm = 0;
    double positron_range_sigma_cm = 0;
    double positronC = 0;
    double positronK1 = 0;
    double positronK2 = 0;
    static VectorR3 PositronRangeLevin(const VectorR3 & p, double positronC,
                                       double positronK1, double positronK2,
                                       double positronMaxRange);
    static VectorR3 PositronRangeGauss(const VectorR3 & p,
                                       double positron_range_sigma,
                                       double positron_max_range);
    virtual double _ExpectedNoPhotons() const;

    double acolinearity = 0;
    double gamma_decay_energy = 0;
    double positron_emission_prob = 1.0;
    bool emit_gamma = false;
    constexpr static double mm_to_cm = 0.1;
};

#endif /* POSITRON_H */
