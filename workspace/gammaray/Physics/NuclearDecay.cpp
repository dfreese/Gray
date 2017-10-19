#include <Physics/NuclearDecay.h>
#include <Random/Random.h>

NuclearDecay::NuclearDecay() :
    energy(0),
    decay_number(0),
    src_id(0),
    position(0, 0, 0),
    time(0)
{
}

Photon NuclearDecay::NextPhoton()
{
    Photon val = daughter.top();
    daughter.pop();
    return (val);
}

bool NuclearDecay::IsEmpty() const
{
    return(daughter.empty());
}

void NuclearDecay::AddPhoton(Photon && p)
{
    daughter.push(p);
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

void NuclearDecay::Reset() {
    while (!daughter.empty()) {
        daughter.pop();
    }
}
