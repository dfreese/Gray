/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Physics/Positron.h"
#include "Gray/Physics/Physics.h"
#include "Gray/Random/Random.h"
#include "Gray/Random/Transform.h"

Positron::Positron(double acolinearity_deg_fwhm, double half_life,
                   double positron_emis_prob, double gamma_decay_energy_mev) :
    Isotope(half_life),
    use_positron_dbexp(false),
    use_positron_gauss(false),
    acolinearity(acolinearity_deg_fwhm / 180.0 * M_PI * Transform::fwhm_to_sigma),
    gamma_decay_energy(gamma_decay_energy_mev),
    positron_emission_prob(positron_emis_prob),
    emit_gamma(gamma_decay_energy_mev > 0)
{
}

std::unique_ptr<Isotope> Positron::Clone() {
    return (std::unique_ptr<Isotope>(new Positron(*this)));
}

NuclearDecay Positron::Decay(int photon_number, double time, int src_id,
                     const VectorR3 & position) const
{
    VectorR3 anni_position(position);
    if (use_positron_dbexp) {
        const double range = Random::TruncatedLevinDoubleExp(
                positronC, positronK1, positronK2, positron_range_max_cm);
        anni_position += range * Random::UniformSphere();
    } else if (use_positron_gauss) {
        const double range = Random::TruncatedGaussian(positron_range_sigma_cm,
                                                       positron_range_max_cm);
        anni_position += range * Random::UniformSphere();
    }
    // TODO: log the positron annihilation and nuclear decay positions
    // separately
    NuclearDecay p(photon_number, time, src_id, anni_position, 0.120);

    if (emit_gamma) {
        // TODO: correctly set the time on the gamma decay, based on the
        // lifetime of the intermediate decay state.
        p.AddPhoton(Photon(position, Random::UniformSphere(),
                           gamma_decay_energy, time, photon_number,
                           Photon::P_YELLOW, src_id));
    }

    // Check to see if a Positron was emitted with the gamma or not.
    if (Random::Selection(positron_emission_prob)) {
        const VectorR3 dir = Random::UniformSphere();
        p.AddPhoton(Photon(anni_position, dir,
                           Physics::energy_511, time, photon_number,
                           Photon::P_BLUE, src_id));
        p.AddPhoton(Photon(anni_position,
                           Random::Acolinearity(dir, acolinearity),
                           Physics::energy_511, time, photon_number,
                           Photon::P_RED, src_id));
    }

    return (p);
}

void Positron::SetPositronRange(double c, double k1, double k2, double max) {
    use_positron_dbexp = true;
    use_positron_gauss  = false;

    // generate cprime which is the scales the dual exponential into a form
    // that allows it to be monte-carlo generated.  It is the integral of
    // exponential k1 portion over the integral of total.
    positronC = c / (c + k1 / k2 * (1 - c));
    positronK1 = k1 / mm_to_cm;
    positronK2 = k2 / mm_to_cm;
    positron_range_max_cm = max * mm_to_cm;
}

void Positron::SetPositronRange(double fwhm, double max) {
    use_positron_dbexp = true;
    use_positron_gauss  = false;
    positron_range_sigma_cm = fwhm * mm_to_cm * Transform::fwhm_to_sigma;
    positron_range_max_cm = max * mm_to_cm;
}

double Positron::ExpectedNoPhotons() const {
    double expected = 2.0 * positron_emission_prob;
    if (emit_gamma) {
        expected += 1.0;
    }
    return(expected);
}
