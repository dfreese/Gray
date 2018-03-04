/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef PHYSICS_H
#define PHYSICS_H

namespace Physics {
    double KleinNishinaEnergy(const double energy, const double theta);
    constexpr double speed_of_light_cmpers = 29979245800.0;
    constexpr double inverse_speed_of_light = (1.0 / speed_of_light_cmpers);
    constexpr double energy_511 = 0.510998903;
    constexpr double decays_per_microcurie = 37.0e3;
};

#endif // PHYSICS_H
