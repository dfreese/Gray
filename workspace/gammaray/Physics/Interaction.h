#ifndef INTERACTION_H
#define INTERACTION_H

#include <map>
#include <VrMath/LinearR3.h>

class Interaction {
public:
    Interaction() {};
    static void MergeStats(Interaction & i0, const Interaction & i1);

    int type = 0;
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
