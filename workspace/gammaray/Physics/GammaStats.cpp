/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Physics/GammaStats.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Math/Math.h>
#include <Physics/Interaction.h>
#include <Physics/Physics.h>
#include <Random/Random.h>

using namespace std;

GammaStats::GammaStats() :
    // Make the form factor always 1
    compton_scatter({0.0, 1.0}, {1.0, 1.0}),
    rayleigh_scatter({0.0, 1.0}, {1.0, 1.0})
{
}

GammaStats::GammaStats(
    double density, std::vector<double> energy,
    std::vector<double> matten_comp, std::vector<double> matten_phot,
    std::vector<double> matten_rayl, std::vector<double> x,
    std::vector<double> form_factor, std::vector<double> scattering_func) :
        energy(energy),
        photoelectric(matten_phot),
        compton(matten_comp),
        rayleigh(matten_rayl),
        log_energy(energy.size()),
        log_photoelectric(matten_phot.size()),
        log_compton(matten_comp.size()),
        log_rayleigh(matten_rayl.size()),
        x(x),
        form_factor(form_factor),
        scattering_func(scattering_func),
        compton_scatter(x, scattering_func),
        rayleigh_scatter(x, form_factor)
{
    // Convert the mass attenuation coefficient to a linear attenuation
    // coefficient by multiplying by density.
    std::transform(photoelectric.begin(), photoelectric.end(),
                   photoelectric.begin(),
                   std::bind1st(std::multiplies<double>(),density));
    std::transform(compton.begin(), compton.end(),
                   compton.begin(),
                   std::bind1st(std::multiplies<double>(),density));
    std::transform(rayleigh.begin(), rayleigh.end(),
                   rayleigh.begin(),
                   std::bind1st(std::multiplies<double>(),density));

    // Cache the log values for log interpolation
    auto log_func = [](double & val) { return (std::log(val)); };
    std::transform(energy.begin(), energy.end(),
                   log_energy.begin(), log_func);
    std::transform(photoelectric.begin(), photoelectric.end(),
                   log_photoelectric.begin(), log_func);
    std::transform(compton.begin(), compton.end(),
                   log_compton.begin(), log_func);
    std::transform(rayleigh.begin(), rayleigh.end(),
                   log_rayleigh.begin(), log_func);
}

GammaStats::AttenLengths GammaStats::GetAttenLengths(double e) const {
    // TODO: This function tends to be called twice with the same e value.  Look
    // at caching this in a way that both causes a gain in speed, and no thread
    // contention.
    AttenLengths cache_len;
    size_t idx = Math::interp_index(energy, e);
    const double log_e = std::log(e);
    cache_len.energy = e;
    cache_len.photoelectric =
        std::exp(Math::interpolate(log_energy, log_photoelectric, log_e, idx));
    cache_len.compton =
        std::exp(Math::interpolate(log_energy, log_compton, log_e, idx));
    cache_len.rayleigh =
        std::exp(Math::interpolate(log_energy, log_rayleigh, log_e, idx));
    return (cache_len);
}

void GammaStats::DisableRayleigh() {
    log_rayleigh = std::vector<double>(rayleigh.size(), std::log(0));
    rayleigh = std::vector<double>(rayleigh.size(), 0);
}

void GammaStats::ComptonScatter(Photon& p) const {
    const double costheta = compton_scatter.scatter_angle(p.GetEnergy(), Random::Uniform());
    // After collision the photon loses some energy to the electron
    p.SetEnergy(Physics::KleinNishinaEnergy(p.GetEnergy(), costheta));
    p.SetDir(Random::Deflection(p.GetDir(),costheta));
    p.SetScatterCompton();
}

void GammaStats::RayleighScatter(Photon& p) const {
    const double costheta = rayleigh_scatter.scatter_angle(p.GetEnergy(), Random::Uniform());
    p.SetDir(Random::Deflection(p.GetDir(), costheta));
    // If the photon scatters on a non-detector, it is a scatter, checked
    // inside SetScatter
    p.SetScatterRayleigh();
}

