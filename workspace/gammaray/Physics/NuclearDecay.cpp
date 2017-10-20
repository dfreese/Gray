#include <Physics/NuclearDecay.h>
#include <Random/Random.h>

NuclearDecay::NuclearDecay(int decay_number, double time, int src_id,
                           const VectorR3 & position, double energy) :
    energy(energy),
    decay_number(decay_number),
    src_id(src_id),
    position(position),
    time(time)
{

}

std::vector<Photon>::reverse_iterator NuclearDecay::begin() {
    return (photons.rbegin());
}

std::vector<Photon>::reverse_iterator NuclearDecay::end() {
    return (photons.rend());
}

void NuclearDecay::AddPhoton(Photon && p)
{
    photons.push_back(p);
}

double NuclearDecay::GetEnergy() const {
    return(energy);
}

int NuclearDecay::GetDecayNumber() const {
    return(decay_number);
}

int NuclearDecay::GetSourceId() const {
    return(src_id);
}

VectorR3 NuclearDecay::GetPosition() const {
    return(position);
}

double NuclearDecay::GetTime() const {
    return(time);
}
