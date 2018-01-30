/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef BEAM_H
#define BEAM_H

#include <Physics/Isotope.h>

class Beam : public Isotope
{
public:
    Beam();
    Beam(const Beam&) = default;
    Beam(const VectorR3 & axis, double angle_deg_fwhm, double energy);

    std::unique_ptr<Isotope> Clone() override;
    NuclearDecay Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position) const override;
    double ExpectedNoPhotons() const override;

private:
    VectorR3 beam_axis;
    double beam_angle;
    double beam_energy;
};

#endif /* BEAM_H */
