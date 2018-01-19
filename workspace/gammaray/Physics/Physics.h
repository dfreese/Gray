/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <Physics/Photon.h>
#include <Physics/Interaction.h>

class GammaStats;
class NuclearDecay;

class Physics {
public:
    static double KleinNishinaEnergy(const double energy, const double theta);
    static constexpr double speed_of_light_cmpers = 29979245800.0;
    static constexpr double inverse_speed_of_light = (1.0 / speed_of_light_cmpers);
    static constexpr double energy_511 = 0.510998903;
    static constexpr double decays_per_microcurie = 37.0e3;
};

#endif // PHYSICS_H
