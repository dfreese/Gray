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
    blue.Reset();
    red.Reset();
    this->src_id = src_id;
    this->time = time;
    this->position = position;
    this->decay_number = photon_number;

    decay_number = photon_number;

    blue.time = time;
    blue.pos = position;
    blue.energy = ENERGY_511;
    blue.id = photon_number;
    blue.det_id = -1;
    blue.src_id = src_id;
    red = blue;

    blue.SetBlue();
    red.SetRed();

    blue.dir.SetUnitZ();
    // Only randomly generate an angle if there's a non zero angle.
    if (angle) {
        Random::Acolinearity(axis, blue.dir, angle);
    }
    red.dir = blue.dir;
    red.dir.Negate();
    AddPhoton(&blue);
    AddPhoton(&red);
}

void BeamDecay::Reset()
{
    position.SetZero();
    blue.SetBlue();
    red.SetRed();

    while (!daughter.empty()) {
        daughter.pop();
    }
}
