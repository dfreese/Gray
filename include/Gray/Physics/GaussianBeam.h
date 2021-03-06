/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef GAUSSIANBEAM_H
#define GAUSSIANBEAM_H

#include "Gray/Physics/Isotope.h"

class GaussianBeam : public Isotope
{
public:
    GaussianBeam();
    GaussianBeam(const GaussianBeam&) = default;
    GaussianBeam(const VectorR3 & axis, double angle_deg_fwhm, double energy);

    std::unique_ptr<Isotope> Clone() override;
    NuclearDecay Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position) const override;
    double ExpectedNoPhotons() const override;
    bool operator==(const GaussianBeam&) const;

private:
    VectorR3 beam_axis;
    double beam_angle;
    double beam_energy;
};

#endif /* GAUSSIANBEAM_H */
