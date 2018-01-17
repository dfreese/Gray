#include <algorithm>
#include <cmath>
#include <exception>
#include <sstream>
#include <vector>
#include <Daq/MergeFunctors.h>

namespace MergeFunctors {

void MergeFirst::operator() (EventT & e0, EventT & e1) const {
    Interaction::MergeStats(e0, e1);
    e0.energy = e0.energy + e1.energy;
    e1.dropped = true;
}


void MergeMax::operator() (EventT & e0, EventT & e1) const {
    if (e0.energy < e1.energy) {
        Interaction::MergeStats(e1, e0);
        e1.energy = e0.energy + e1.energy;
        e0.dropped = true;
    } else {
        Interaction::MergeStats(e0, e1);
        e0.energy = e0.energy + e1.energy;
        e1.dropped = true;
    }
}

MergeAnger::MergeAnger(const std::vector<DetIdT> & base,
                const std::vector<DetIdT> & bx,
                const std::vector<DetIdT> & by,
                const std::vector<DetIdT> & bz) :
    base(base),
    bx(bx),
    by(by),
    bz(bz),
    no_blk(*std::max_element(base.begin(), base.end()) + 1),
    no_bx(*std::max_element(bx.begin(), bx.end()) + 1),
    no_by(*std::max_element(by.begin(), by.end()) + 1),
    no_bz(*std::max_element(bz.begin(), bz.end()) + 1),
    reverse_map(create_reverse_map())
{
}

std::vector<DetIdT> MergeAnger::create_reverse_map() const {
    const int total = static_cast<int>(base.size());
    const int implied_total = no_blk * no_bx * no_by * no_bz;
    if (total != implied_total) {
        std::stringstream err;
        err << total << " detectors specified, but anger logic mapping of "
        << no_blk << " blocks with a size of (x,y,z) = (" << no_bx
        << "," << no_by << "," << no_bz << ")" << " implies "
        << implied_total << " detectors.";
        throw std::runtime_error(err.str());

    }
    std::vector<int> reverse_map(total, -1);

    for (int idx = 0; idx < total; idx++) {
        int rev_idx = index(base[idx], bx[idx], by[idx], bz[idx]);
        if ((rev_idx < 0) || (rev_idx >= total)) {
            std::stringstream err;
            err << "Block index mapping is not consistent with block size "
            << "(x,y,z) = (" << no_bx << "," << no_by << "," << no_bz
            << ")" << " at detector " << idx;
            throw std::runtime_error(err.str());
        }
        if (reverse_map[rev_idx] != -1) {
            std::stringstream err;
            err << "Duplicate mapping found for anger merge with block"
            << "size (x,y,z) = (" << no_bx << "," << no_by << ","
            << no_bz << ")" << " at detector " << idx;
            throw std::runtime_error(err.str());
        }
        reverse_map[rev_idx] = idx;
    }
    return (reverse_map);
}

int MergeAnger::index(int blk, int bx, int by, int bz) const {
    return (((blk * no_bz + bz) * no_by + by) * no_bx + bx);
}

void MergeAnger::operator() (EventT & e0, EventT & e1) const {
    const float energy_result = e0.energy + e1.energy;
    // Base is inherently the same for both detectors inherently by being
    // matched in merge.
    const int blk = base[e0.det_id];
    const int row0 = bx[e0.det_id];
    const int row1 = bx[e1.det_id];
    const int col0 = by[e0.det_id];
    const int col1 = by[e1.det_id];
    const int lay0 = bz[e0.det_id];
    const int lay1 = bz[e1.det_id];
    const int row_result = static_cast<int>(std::round(
        static_cast<float>(row0) * (e0.energy / energy_result) +
        static_cast<float>(row1) * (e1.energy / energy_result)));
    const int col_result = static_cast<int>(std::round(
        static_cast<float>(col0) * (e0.energy / energy_result) +
        static_cast<float>(col1) * (e1.energy / energy_result)));
    const int lay_result = static_cast<int>(std::round(
        static_cast<float>(lay0) * (e0.energy / energy_result) +
        static_cast<float>(lay1) * (e1.energy / energy_result)));

    const int rev_idx = index(blk, row_result, col_result, lay_result);
    const int id_result = reverse_map[rev_idx];
    if (e0.energy < e1.energy) {
        Interaction::MergeStats(e1, e0);
        e1.det_id = id_result;
        e1.energy = energy_result;
        e0.dropped = true;
    } else {
        Interaction::MergeStats(e0, e1);
        e0.det_id = id_result;
        e0.energy = energy_result;
        e1.dropped = true;
    }
}
}
