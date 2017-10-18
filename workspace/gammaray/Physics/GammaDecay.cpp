#include <Physics/GammaDecay.h>
#include <stdio.h>
#include <math.h>
#include <Random/Random.h>

using namespace std;

GammaDecay::GammaDecay() :
    energy(0),
    gamma()
{
}


void GammaDecay::Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position)
{
    this->position = position;
    this->decay_number = photon_number;
    this->time = time;
    this->src_id = src_id;
    gamma = Photon(position, Random::UniformSphere(), energy,
                   time, photon_number, Photon::P_YELLOW, src_id);
    AddPhoton(&gamma);
}

void GammaDecay::Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position, double energy)
{
    this->energy = energy;
    Decay(photon_number, time, src_id, position);
}
