/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Physics/Compton.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include "Gray/Math/Math.h"
#include "Gray/Physics/KleinNishina.h"

Compton::Compton(
        const std::vector<double>& x,
        const std::vector<double>& form_factor) :
    // These energies were chosen, as they give less than 0.5% error from 0 to
    // 1.5MeV when linear interpolation is performed.
    energy_idx({
        0.0, 0.010, 0.030, 0.050, 0.100, 0.200, 0.300, 0.400, 0.500, 0.600,
        0.700, 0.900, 1.100, 1.300, 1.500}),
    // Go from -1 to 1 linear in theta
    costheta_idx(Math::cos_space(300)),
    scatter_cdfs(create_scatter_cdfs(energy_idx, costheta_idx, x, form_factor))
{
}

double Compton::scatter_angle(double energy, double rand_uniform) const {
    return (Math::interpolate_y_2d(energy_idx, costheta_idx, scatter_cdfs,
                                   energy, rand_uniform));
}

double Compton::x_val(double cos_theta, double energy_mev) {
    // Planck's contant times the speed of light in MeV*cm
    constexpr double hc_MeV_cm = 1.23984193e-10;
    return (std::sqrt(0.5 * (1.0 - cos_theta)) * energy_mev / hc_MeV_cm);
}

std::vector<double> Compton::x_val(
        const std::vector<double>& costhetas,
        const double energy_mev)
{
    std::vector<double> dsigma_dtheta(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), dsigma_dtheta.begin(),
                   [&energy_mev](double costheta) {
                       return (x_val(costheta, energy_mev));
                   });
    return (dsigma_dtheta);
}

std::vector<double> Compton::scattering(
        const std::vector<double>& costhetas,
        const double energy_mev,
        const std::vector<double>& x,
        const std::vector<double>& scattering_func)
{
    std::vector<double> scattering_vals = x_val(costhetas, energy_mev);
    std::transform(scattering_vals.begin(), scattering_vals.end(),
                   scattering_vals.begin(),
                   [&x, &scattering_func](double x_val) {
                       return (Math::interpolate(x, scattering_func, x_val));
                   });
    return (scattering_vals);
}

std::vector<double> Compton::dsigma(
        const std::vector<double>& costhetas,
        const double energy_mev,
        const std::vector<double>& x,
        const std::vector<double>& scattering_func)
{
    std::vector<double> dsigma_dtheta(
            KleinNishina::dsigma(costhetas, energy_mev));
    std::vector<double> scattering_vals(
            scattering(costhetas, energy_mev, x, scattering_func));
    // Multiply all of the dsigma_dtheta vals by the respective scattering func
    // value.
    auto sf_val = scattering_vals.begin();
    for (double & ds_val : dsigma_dtheta) {
        ds_val *= *(sf_val++);
    }
    return (dsigma_dtheta);
}

std::vector<std::vector<double>> Compton::create_scatter_cdfs(
        const std::vector<double>& energies,
        const std::vector<double>& costhetas,
        const std::vector<double>& x,
        const std::vector<double>& scattering_func)
{
    // Integrate in theta space, not cos(theta).
    std::vector<double> thetas(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), thetas.begin(),
                   [](double theta) { return (std::acos(theta)); });

    std::vector<std::vector<double>> scatter_cdfs(energies.size());
    for (size_t ii = 0; ii < energies.size(); ++ii) {
        const double energy = energies[ii];
        auto & energy_cdf = scatter_cdfs[ii];
        energy_cdf = dsigma(costhetas, energy, x, scattering_func);
        energy_cdf = Math::pdf_to_cdf(thetas, energy_cdf);
    }

    return (scatter_cdfs);
}
