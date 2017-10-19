#include <Physics/BeamDecay.h>
#include <Physics/Photon.h>
#include <Physics/Physics.h>
#include <Random/Random.h>

BeamDecay::BeamDecay()
{
}

void BeamDecay::Decay(int photon_number, double time, int src_id,
                      const VectorR3 & position)
{
    this->energy = 0;
    this->src_id = src_id;
    this->time = time;
    this->position = position;
    this->decay_number = photon_number;
}
