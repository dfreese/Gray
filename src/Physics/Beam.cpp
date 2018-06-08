/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Physics/Beam.h"
#include "Gray/Physics/NuclearDecay.h"
#include "Gray/Physics/Physics.h"
#include "Gray/Random/Random.h"
#include <limits>

using namespace std;

Beam::Beam() :
    Beam({0, 0, 1}, 0, Physics::energy_511)
{
}

Beam::Beam(const VectorR3 & axis, double angle_max_deg, double energy) :
    Isotope(std::numeric_limits<double>::infinity()),
    beam_axis(axis),
    beam_angle_max((angle_max_deg / 180.0) * M_PI),
    beam_energy(energy)
{

}

std::unique_ptr<Isotope> Beam::Clone() {
    return (std::unique_ptr<Isotope>(new Beam(*this)));
}

NuclearDecay Beam::Decay(int photon_number, double time, int src_id,
                 const VectorR3 & position) const
{
    NuclearDecay beam(photon_number, time, src_id, position, 0);

    VectorR3 dir;
    // Only randomly generate an angle if there's a non zero angle.
    if (beam_angle_max) {
        dir = Random::DeflectionUniform(beam_axis, beam_angle_max);
    } else {
        dir = beam_axis;
    }
    beam.AddPhoton(Photon(position, dir, beam_energy,
                          time, photon_number, Photon::P_BLUE, src_id));
    beam.AddPhoton(Photon(position, dir.Negate(), beam_energy,
                          time, photon_number, Photon::P_RED, src_id));
    return (beam);
}

double Beam::ExpectedNoPhotons() const {
    return(2.0);
}

bool Beam::operator==(const Beam& rhs) const {
    return (beam_axis == rhs.beam_axis) &&
        (beam_angle_max == rhs.beam_angle_max) &&
        (beam_energy == rhs.beam_energy);
}
