/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef GAMMAPOSITRON_H
#define GAMMAPOSITRON_H

#include "Gray/Physics/Isotope.h"

class Positron: public Isotope
{
public:
    Positron() = default;
    Positron(const Positron&) = default;
    Positron(double acolinearity_deg_fwhm, double half_life,
             double gamma_decay_energy_mev, double positron_emis_prob);
    std::unique_ptr<Isotope> Clone() override;
    NuclearDecay Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position) const override;
    double ExpectedNoPhotons() const override;
    void SetPositronRange(double c, double k1, double k2, double max);
    void SetPositronRange(double fwhm_mm, double max_mm);

private:
    bool use_positron_dbexp = false;
    bool use_positron_gauss = false;
    double positron_range_max_cm = 0;
    double positron_range_sigma_cm = 0;
    double positronC = 0;
    double positronK1 = 0;
    double positronK2 = 0;

    double acolinearity = 0;
    double gamma_decay_energy = 0;
    double positron_emission_prob = 1.0;
    bool emit_gamma = false;
    constexpr static double mm_to_cm = 0.1;
};

#endif /* POSITRON_H */
