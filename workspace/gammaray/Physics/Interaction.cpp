/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Physics/Interaction.h>
#include <cmath>
#include <iostream>
#include <Physics/GammaStats.h>
#include <Physics/Photon.h>
#include <Physics/NuclearDecay.h>

using namespace std;

Interaction::Interaction(
        Type type,
        const Photon& p) :
    type(type),
    decay_id(p.GetId()),
    time(p.GetTime()),
    pos(p.GetPos()),
    energy(p.GetEnergy()),
    color(p.GetColor()),
    src_id(p.GetSrc()),
    mat_id(-1),
    det_id(p.GetDetId()),
    scatter_compton_phantom(p.GetScatterComptonPhantom()),
    scatter_compton_detector(p.GetScatterComptonDetector()),
    scatter_rayleigh_phantom(p.GetScatterRayleighPhantom()),
    scatter_rayleigh_detector(p.GetScatterRayleighDetector()),
    xray_flouresence(p.GetXrayFlouresence()),
    dropped(true)
{
}

Interaction::Interaction(
        Type type,
        const Photon& p,
        const GammaStats& mat,
        double deposit) :
    type(type),
    decay_id(p.GetId()),
    time(p.GetTime()),
    pos(p.GetPos()),
    energy(deposit),
    color(p.GetColor()),
    src_id(p.GetSrc()),
    mat_id(mat.GetId()),
    det_id(p.GetDetId()),
    scatter_compton_phantom(p.GetScatterComptonPhantom()),
    scatter_compton_detector(p.GetScatterComptonDetector()),
    scatter_rayleigh_phantom(p.GetScatterRayleighPhantom()),
    scatter_rayleigh_detector(p.GetScatterRayleighDetector()),
    xray_flouresence(p.GetXrayFlouresence()),
    dropped(Dropped(type, mat))
{
}

Interaction::Interaction(
        const NuclearDecay& p,
        const GammaStats & mat) :
    type(Type::NUCLEAR_DECAY),
    decay_id(p.GetDecayNumber()),
    time(p.GetTime()),
    pos(p.GetPosition()),
    energy(p.GetEnergy()),
    color(Photon::Color::P_YELLOW),
    src_id(p.GetSourceId()),
    mat_id(mat.GetId()),
    det_id(-1),
    scatter_compton_phantom(0),
    scatter_compton_detector(0),
    scatter_rayleigh_phantom(0),
    scatter_rayleigh_detector(0),
    xray_flouresence(0),
    dropped(Dropped(type, mat))
{
}



/*!
 * Performs the basic merging required for different events.  Merges energy and
 * stats, forcing other information to be modified elsewhere.
 *
 * Stats in interactions, such as scatter_compton_phantom accumulate for the
 * photon as it propogates through the material.  In order to keep these stats
 * accurate, we need to keep a record of all the hits that have been merged in
 * and then recreate the stats based on this record.
 */
void Interaction::MergeStats(Interaction & i0, const Interaction & i1) {
    if (i0.merged_hits.empty()) {
        // The event has not been through a merge process, so create an info
        // struct for its decay/color pair, and add in the info
        MergedEventsInfo & info = i0.merged_hits[{i0.decay_id, i0.color}];
        info.scatter_compton_phantom = i0.scatter_compton_phantom;
        info.scatter_compton_detector = i0.scatter_compton_detector;
        info.scatter_rayleigh_phantom = i0.scatter_rayleigh_phantom;
        info.scatter_rayleigh_detector = i0.scatter_rayleigh_detector;
        info.xray_flouresence = i0.xray_flouresence;
    }

    if (i1.merged_hits.empty()) {
        // The constant event has not been through a merge process, so find or
        // create an info struct for its decay/color pair, and add in the info
        MergedEventsInfo & info = i0.merged_hits[{i1.decay_id, i1.color}];
        info.scatter_compton_phantom = max(i1.scatter_compton_phantom,
                                           info.scatter_compton_phantom);
        info.scatter_compton_detector = max(i1.scatter_compton_detector,
                                            info.scatter_compton_detector);
        info.scatter_rayleigh_phantom = max(i1.scatter_rayleigh_phantom,
                                            info.scatter_rayleigh_phantom);
        info.scatter_rayleigh_detector = max(i1.scatter_rayleigh_detector,
                                             info.scatter_rayleigh_detector);
        info.xray_flouresence = max(i1.xray_flouresence,
                                    info.xray_flouresence);
    } else {
        for (const auto & hits_pair: i1.merged_hits) {
            const auto & hit_id = hits_pair.first;
            const MergedEventsInfo & hit_info = hits_pair.second;
            MergedEventsInfo & info = i0.merged_hits[hit_id];
            info.scatter_compton_phantom = max(hit_info.scatter_compton_phantom,
                                               info.scatter_compton_phantom);
            info.scatter_compton_detector = max(hit_info.scatter_compton_detector,
                                                info.scatter_compton_detector);
            info.scatter_rayleigh_phantom = max(hit_info.scatter_rayleigh_phantom,
                                                info.scatter_rayleigh_phantom);
            info.scatter_rayleigh_detector = max(hit_info.scatter_rayleigh_detector,
                                                 info.scatter_rayleigh_detector);
            info.xray_flouresence = max(hit_info.xray_flouresence,
                                        info.xray_flouresence);
        }
    }

    i0.scatter_compton_phantom = 0;
    i0.scatter_compton_detector = 0;
    i0.scatter_rayleigh_phantom = 0;
    i0.scatter_rayleigh_detector = 0;
    i0.xray_flouresence = 0;
    for (const auto & hits_pair: i0.merged_hits) {
        const MergedEventsInfo & hit_info = hits_pair.second;
        i0.scatter_compton_phantom += hit_info.scatter_compton_phantom;
        i0.scatter_compton_detector += hit_info.scatter_compton_detector;
        i0.scatter_rayleigh_phantom += hit_info.scatter_rayleigh_phantom;
        i0.scatter_rayleigh_detector += hit_info.scatter_rayleigh_detector;
        i0.xray_flouresence += hit_info.xray_flouresence;
    }
}

/*!
 * This determines if the DaqModel will try and process this event or not.
 * We can keep the interactions, such as errors in the buffer around to log them
 * as hits, but they will not be processed as singles.
 */
bool Interaction::Dropped(Type type, const GammaStats& mat) {
    switch (type) {
        case Type::COMPTON:
            return (!mat.IsSensitive());
        case Type::PHOTOELECTRIC:
            return (!mat.IsSensitive());
        case Type::RAYLEIGH:
            return (true);
        case Type::NUCLEAR_DECAY:
            return (true);
        case Type::ERROR_EMPTY:
            return (true);
        case Type::ERROR_TRACE_DEPTH:
            return (true);
        case Type::ERROR_MATCH:
            return (true);
    }
}
