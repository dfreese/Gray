/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Physics/Isotope.h>
#include <cmath>

Isotope::Isotope(double half_life_s) :
    half_life(half_life_s)
{
}

double Isotope::GetHalfLife() const {
    return(half_life);
}

double Isotope::FractionRemaining(double time) const {
    // Note: this will produce a nan at time=infinity for infinite half_life.
    // Please don't do that....
    return(std::pow(0.5, time / half_life));
}

double Isotope::FractionIntegral(double start, double time) const {
    if (half_life == std::numeric_limits<double>::infinity()) {
        return (time);
    } else {
        return (half_life / std::log(2.0) *
                (FractionRemaining(start) - FractionRemaining(start + time)));
    }
}
