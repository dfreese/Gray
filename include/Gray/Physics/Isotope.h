/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <limits>
#include <memory>
#include "Gray/VrMath/LinearR3.h"
#include "Gray/Physics/NuclearDecay.h"

class Isotope
{
public:
    Isotope() = default;
    Isotope(double half_life_s);
    virtual ~Isotope() = default;
    virtual std::unique_ptr<Isotope> Clone() = 0;
    virtual NuclearDecay Decay(int photon_number, double time, int src_id,
                               const VectorR3 & position) const = 0;
    double GetHalfLife() const;
    void DisableHalfLife();
    double FractionRemaining(double time) const;
    double FractionIntegral(double start, double time) const;
    virtual double ExpectedNoPhotons() const = 0;

private:
    double half_life = std::numeric_limits<double>::infinity();
};

#endif /* ISOTOPE_H */
