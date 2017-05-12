#include <Physics/Interaction.h>
#include <cmath>
#include <iostream>

using namespace std;

/*!
 * Performs the basic merging required for different events.  Merges energy and
 * stats, forcing other information to be modified elsewhere.
 *
 * Stats in interactions, such as scatter_compton_phantom accumulate for the
 * photon as it propogates through the material.  In order to keep these stats
 * accurate, we need to keep a record of all the hits that have been merged in
 * and then recreate the stats based on this record.
 */
void Interaction::basic_merge(Interaction & i0, const Interaction & i1) {
    // As always, merge the energies
    i0.energy += i1.energy;

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
