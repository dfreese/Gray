/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Physics/GaussianBeam.h"
#include "Gray/Physics/NuclearDecay.h"
#include "Gray/Physics/Physics.h"
#include "Gray/Random/Random.h"
#include <limits>

using namespace std;

GaussianBeam::GaussianBeam() :
    GaussianBeam({0, 0, 1}, 0, Physics::energy_511)
{
}

GaussianBeam::GaussianBeam(const VectorR3 & axis,
        double angle_deg_fwhm, double energy) :
    Isotope(std::numeric_limits<double>::infinity()),
    beam_axis(axis),
    beam_angle((angle_deg_fwhm/180.0) * M_PI/2.35482005),
    beam_energy(energy)
{

}

std::unique_ptr<Isotope> GaussianBeam::Clone() {
    return (std::unique_ptr<Isotope>(new GaussianBeam(*this)));
}

NuclearDecay GaussianBeam::Decay(int photon_number, double time, int src_id,
                 const VectorR3 & position) const
{
    NuclearDecay beam(photon_number, time, src_id, position, 0);

    VectorR3 dir;
    // Only randomly generate an angle if there's a non zero angle.
    if (beam_angle) {
        dir = Random::Acolinearity(beam_axis, beam_angle);
    } else {
        dir = beam_axis;
    }
    beam.AddPhoton(Photon(position, dir, beam_energy,
                          time, photon_number, Photon::P_BLUE, src_id));
    beam.AddPhoton(Photon(position, dir.Negate(), beam_energy,
                          time, photon_number, Photon::P_RED, src_id));
    return (beam);
}

double GaussianBeam::ExpectedNoPhotons() const {
    return(2.0);
}

bool GaussianBeam::operator==(const GaussianBeam& rhs) const {
    return (beam_axis == rhs.beam_axis) &&
        (beam_angle == rhs.beam_angle) &&
        (beam_energy == rhs.beam_energy);
}
