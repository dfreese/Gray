#ifndef NUCLEARDECAY_H
#define NUCLEARDECAY_H

#include <vector>
#include <Physics/Photon.h>
#include <VrMath/LinearR3.h>

class NuclearDecay
{
public:
    NuclearDecay() = default;
    NuclearDecay(int decay_number, double time, int src_id,
                 const VectorR3 & position, double energy);
    Photon NextPhoton();
    bool IsEmpty() const;
    double GetEnergy() const;
    int GetDecayNumber() const;
    int GetSourceId() const;
    VectorR3 GetPosition() const;
    double GetTime() const;
    void AddPhoton(Photon && p);
    std::vector<Photon>::const_reverse_iterator begin() const;
    std::vector<Photon>::const_reverse_iterator end() const;

private:
    double energy = 0;
    int decay_number = 0;
    int src_id = 0;
    VectorR3 position = {0, 0, 0};
    double time = 0;
    std::vector<Photon> photons;
};

#endif /* NUCLEARDECAY_H */
