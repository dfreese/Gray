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
    gamma.time = time;
    gamma.pos = position;
    gamma.energy = energy;
    gamma.id = photon_number;
    gamma.det_id = -1;
    gamma.color = Photon::P_YELLOW;
    gamma.dir.SetUnitZ();
    gamma.src_id = src_id;
    gamma.phantom_scatter = false;
    Random::UniformSphere(gamma.dir);
    AddPhoton(&gamma);
}

void GammaDecay::Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position, double energy)
{
    this->energy = energy;
    Decay(photon_number, time, src_id, position);
}
