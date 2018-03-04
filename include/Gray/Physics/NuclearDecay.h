/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef NUCLEARDECAY_H
#define NUCLEARDECAY_H

#include <vector>
#include "Gray/Physics/Photon.h"
#include "Gray/VrMath/LinearR3.h"

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
