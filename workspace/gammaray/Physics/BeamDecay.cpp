#include <Physics/BeamDecay.h>
#include <Random/Random.h>
#include <Physics/Physics.h>

BeamDecay::BeamDecay() :
    angle(0),
    axis(0, 0, 0)
{
}

void BeamDecay::SetBeam(const VectorR3 &axis, const double angle)
{
    this->axis = axis;
    this->angle = angle;
}

void BeamDecay::Decay(int photon_number, double time, int src_id,
                      const VectorR3 & position)
{
    this->energy = 0;
    this->src_id = src_id;
    this->time = time;
    this->position = position;
    this->decay_number = photon_number;
    
    VectorR3 dir;
    // Only randomly generate an angle if there's a non zero angle.
    if (angle) {
        dir = Random::Acolinearity(axis, angle);
    } else {
        dir = axis;
    }

    blue = Photon(position, dir, Physics::energy_511,
                  time, photon_number, Photon::P_BLUE, src_id);
    red = Photon(position, dir.Negate(), Physics::energy_511,
                 time, photon_number, Photon::P_RED, src_id);
    AddPhoton(&blue);
    AddPhoton(&red);
}
