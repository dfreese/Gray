/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Physics/Physics.h>

double Physics::KleinNishinaEnergy(const double energy, const double costheta)
{
    return(energy / (1.0 + (energy / Physics::energy_511) * (1. - costheta)));
}
