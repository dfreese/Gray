/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Daq/BlurFunctors.h"
#include "Gray/Random/Random.h"

namespace BlurFunctors {

BlurEnergy::BlurEnergy(double fwhm_percent) :
    value(fwhm_percent)
{
}

void BlurEnergy::operator() (EventT & event) const {
    event.energy = Random::GaussianEnergyBlur(event.energy, value);
}

BlurEnergyReferenced::BlurEnergyReferenced(double fwhm_percent,
                                           double ref_energy) :
    value(fwhm_percent),
    ref(ref_energy)
{
}

void BlurEnergyReferenced::operator() (EventT & event) const {
    event.energy = Random::GaussianEnergyBlurInverseSqrt(event.energy,
                                                         value, ref);
}

BlurTime::BlurTime(double fwhm_time, double max_blur) :
    value(fwhm_time),
    max(max_blur)
{
}

void BlurTime::operator() (EventT & event) const {
    event.time = Random::GaussianBlurTimeTrunc(event.time, value, max);
}
}
