#include <Physics/Beam.h>
#include <Physics/NuclearDecay.h>
#include <Physics/Physics.h>
#include <Random/Random.h>
#include <limits>

using namespace std;

Beam::Beam() :
    Beam({0, 0, 1}, 0, Physics::energy_511)
{
}

Beam::Beam(const VectorR3 & axis, double angle, double energy) :
    Isotope(std::numeric_limits<double>::infinity()),
    beam_axis(axis),
    beam_angle(angle),
    beam_energy(energy)
{

}

std::unique_ptr<Isotope> Beam::Clone() {
    return (std::unique_ptr<Isotope>(new Beam(*this)));
}

void Beam::SetBeam(const VectorR3 & axis, double angle, double energy)
{
    beam_axis = axis;
    beam_angle = angle;
    beam_energy = energy;
}

NuclearDecay Beam::Decay(int photon_number, double time, int src_id,
                 const VectorR3 & position)
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

double Beam::ExpectedNoPhotons() const {
    return(2.0);
}
