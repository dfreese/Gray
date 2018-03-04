/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef RAYLEIGH_H
#define RAYLEIGH_H

#include <vector>

class Rayleigh {
public:
    Rayleigh(const std::vector<double>& x,
            const std::vector<double>& form_factor);
    static std::vector<double> formfactor(
            const std::vector<double>& costheta,
            const double energy_mev,
            const std::vector<double>& x,
            const std::vector<double>& form_factor);
    static std::vector<double> dsigma(
            const std::vector<double>& costheta,
            const double energy_mev,
            const std::vector<double>& x,
            const std::vector<double>& form_factor);
    static std::vector<std::vector<double>> create_scatter_cdfs(
            const std::vector<double>& energies,
            const std::vector<double>& costhetas,
            const std::vector<double>& x,
            const std::vector<double>& form_factor);
    double scatter_angle(double energy, double rand_uniform) const;
private:
    std::vector<double> energy_idx;
    std::vector<double> costheta_idx;
    std::vector<std::vector<double>> scatter_cdfs;
};

#endif // RAYLEIGH_H
