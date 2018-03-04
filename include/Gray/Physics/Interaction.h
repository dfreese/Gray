/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef INTERACTION_H
#define INTERACTION_H

#include <map>
#include "Gray/VrMath/LinearR3.h"
class GammaMaterial;
class Photon;
class NuclearDecay;

class Interaction {
public:
    enum class Type : int {
        COMPTON = 0,
        PHOTOELECTRIC = 1,
        RAYLEIGH = 2,
        NUCLEAR_DECAY = 3,
        ERROR_EMPTY = -1,
        ERROR_TRACE_DEPTH = -2,
        ERROR_MATCH = -3,
    };

    Interaction() = default;
    Interaction(Type type, const Photon& p); // For error creation
    Interaction(Type type, const Photon& p, const GammaMaterial& mat, double deposit);
    Interaction(const NuclearDecay& p, const GammaMaterial& mat);
    static void MergeStats(Interaction & i0, const Interaction & i1);
    static bool Dropped(Type type, const GammaMaterial& mat);

    Type type = Type::ERROR_EMPTY;
    int decay_id = 0;
    double time = 0;
    VectorR3 pos = {0, 0, 0};
    double energy = 0;
    int color = 0;
    int src_id = 0;
    int mat_id = 0;
    int det_id = 0;
    int scatter_compton_phantom = 0;
    int scatter_compton_detector = 0;
    int scatter_rayleigh_phantom = 0;
    int scatter_rayleigh_detector = 0;
    int xray_flouresence = 0;
    int coinc_id = -1;
    bool dropped = false;

    struct MergedEventsInfo {
        int scatter_compton_phantom = 0;
        int scatter_compton_detector = 0;
        int scatter_rayleigh_phantom = 0;
        int scatter_rayleigh_detector = 0;
        int xray_flouresence = 0;
    };
    std::map<std::pair<int, int>, MergedEventsInfo> merged_hits;
};

#endif // INTERACTION_H
