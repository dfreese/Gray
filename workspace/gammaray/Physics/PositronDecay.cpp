#include <Physics/PositronDecay.h>
#include <Physics/Photon.h>
#include <Physics/Physics.h>
#include <Random/Random.h>
#include <Random/Transform.h>

void PositronDecay::Decay(int photon_number, double time, int src_id,
                          const VectorR3 & position)
{
    // 120keV positron energy for FDG
    // TODO: put an energy in the Positron class that is overridden by it's
    // children.  Pass it during the call to PositronDecay::Decay, and
    // generate an energy for the PositronDecay instance randomly from a
    // beta decay spectrum.
    this->energy = 0.120;
    this->decay_number = photon_number;
    this->src_id = src_id;
    this->time = time;
    this->position = position;
}
