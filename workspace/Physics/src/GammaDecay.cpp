#include <Physics/GammaDecay.h>
#include <stdio.h>
#include <math.h>
#include <Random/Random.h>

using namespace std;

GammaDecay::GammaDecay()
{
    gamma.color = Photon::P_YELLOW;
}

void GammaDecay::Reset()
{
    gamma.Reset();
    gamma.color = Photon::P_YELLOW;
    while (!daughter.empty()) {
        daughter.pop();
    }
}

void GammaDecay::Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position)
{
    gamma.Reset();
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
    Random::UniformSphere(gamma.dir);
    AddPhoton(&gamma);
}

void GammaDecay::SetEnergy(double e)
{
    energy = e;
};
