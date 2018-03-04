/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef KLEIN_NISHINA_H
#define KLEIN_NISHINA_H

#include <vector>

class KleinNishina {
public:
    KleinNishina();
    static double dsigma(const double costheta, const double energy_mev);
    static std::vector<double> dsigma(
            const std::vector<double>& costheta,
            const double energy_mev);
    static std::vector<std::vector<double>> create_scatter_cdfs(
            const std::vector<double>& energies,
            const std::vector<double>& costhetas);
    double scatter_angle(double energy, double rand_uniform) const;
private:
    std::vector<double> energy_idx;
    std::vector<double> costheta_idx;
    std::vector<std::vector<double>> scatter_cdfs;
};

#endif // KLEIN_NISHINA_H
