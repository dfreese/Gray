/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Physics/Rayleigh.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include "Gray/Math/Math.h"
#include "Gray/Physics/Compton.h"
#include "Gray/Physics/Thompson.h"

Rayleigh::Rayleigh(
        const std::vector<double>& x,
        const std::vector<double>& form_factor) :
    energy_idx({
        0.0, 0.001, 0.002, 0.005, 0.010, 0.020, 0.040, 0.060, 0.080, 0.080,
        0.100, 0.200, 0.300, 0.500, 1.000}),
    costheta_idx(Math::cos_space(300)),
    scatter_cdfs(create_scatter_cdfs(energy_idx, costheta_idx, x, form_factor))
{
}

double Rayleigh::scatter_angle(
        double energy, double rand_uniform) const
{
    return (Math::interpolate_y_2d(energy_idx, costheta_idx, scatter_cdfs,
                                   energy, rand_uniform));
}

std::vector<double> Rayleigh::formfactor(
        const std::vector<double>& costhetas,
        const double energy_mev,
        const std::vector<double>& x,
        const std::vector<double>& form_factor)
{
    std::vector<double> formfactor_vals(Compton::x_val(costhetas, energy_mev));
    std::transform(formfactor_vals.begin(), formfactor_vals.end(),
                   formfactor_vals.begin(),
                   [&x, &form_factor](double x_val) {
                       return (Math::interpolate(x, form_factor, x_val));
                   });
    return (formfactor_vals);
}

std::vector<double> Rayleigh::dsigma(
        const std::vector<double>& costhetas,
        const double energy_mev,
        const std::vector<double>& x,
        const std::vector<double>& form_factor)
{
    std::vector<double> dsigma_dtheta(Thompson::dsigma(costhetas));
    std::vector<double> formfactor_vals(
        formfactor(costhetas, energy_mev, x, form_factor));
    // Multiply all of the dsigma_dtheta vals by the respective scattering func
    // value.
    auto ff_iter = formfactor_vals.begin();
    for (double & ds_val : dsigma_dtheta) {
        double ff_val = *(ff_iter++);
        ds_val *= ff_val * ff_val;
    }
    return (dsigma_dtheta);
}

std::vector<std::vector<double>> Rayleigh::create_scatter_cdfs(
        const std::vector<double>& energies,
        const std::vector<double>& costhetas,
        const std::vector<double>& x,
        const std::vector<double>& form_factor)
{
    // Integrate in theta space, not cos(theta).
    std::vector<double> thetas(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), thetas.begin(),
                   [](double theta) { return (std::acos(theta)); });

    std::vector<std::vector<double>> scatter_cdfs(energies.size());
    for (size_t ii = 0; ii < energies.size(); ++ii) {
        const double energy = energies[ii];
        auto & energy_cdf = scatter_cdfs[ii];
        energy_cdf = dsigma(costhetas, energy, x, form_factor);
        energy_cdf = Math::pdf_to_cdf(thetas, energy_cdf);
    }

    return (scatter_cdfs);
}
