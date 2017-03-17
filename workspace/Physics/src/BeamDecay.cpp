#include <Physics/BeamDecay.h>
#include <Random/Random.h>

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

    blue.time = time;
    blue.pos = position;
    blue.energy = ENERGY_511;
    blue.id = photon_number;
    blue.det_id = -1;
    blue.src_id = src_id;

    // Only randomly generate an angle if there's a non zero angle.
    if (angle) {
        Random::Acolinearity(axis, blue.dir, angle);
    } else {
        blue.pos = axis;
    }
    red = blue;

    blue.SetBlue();
    red.SetRed();
    red.dir.Negate();
    AddPhoton(&blue);
    AddPhoton(&red);
}
