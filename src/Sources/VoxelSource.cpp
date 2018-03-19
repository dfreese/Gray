/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Sources/VoxelSource.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>
#include "Gray/Random/Random.h"

VoxelSource::VoxelSource(
        const VectorR3& position, const VectorR3& size, const VectorR3& axis,
        double activity) :
    Source(position, activity),
    size(size),
    local_to_global(RefAxisPlusTransToMap(axis, position)),
    global_to_local(local_to_global.Inverse())
{
}

VectorR3 VoxelSource::Decay() const {
    // Since we have created a CDF for the indices, we can do an inversion
    // selection to randomly choose the voxel with the appropriate value. We
    // have already created a sorted array, with 1.0 being the last value, so
    // lower_bound will never select past end() - 1.  So idx ends up being
    // [0, x*y*z - 1].
    auto val = std::lower_bound(prob.begin(), prob.end(), Random::Uniform());
    size_t idx = std::distance(prob.begin(), val);
    // Now, since this was in [x,y,z], c order, calculate the voxel value in
    // each of the dimensions.
    int z = idx % dims[2];
    int y = (idx / dims[2]) % dims[1];
    int x = idx / dims[2] / dims[1];

    // Each dimension is distributed [0, dim - 1], add a random variable to
    // distribute within voxels so that we have [0, dim] as a float.  divide
    // by dim so we have a [0,1].  Subtract 0.5 so we're [-0.5, 0.5].  Scale
    // that by size so we're [-size/2, size/2].
    VectorR3 pos(
            ((x + Random::Uniform()) / dims[0] - 0.5) * size.x,
            ((y + Random::Uniform()) / dims[1] - 0.5) * size.y,
            ((z + Random::Uniform()) / dims[2] - 0.5) * size.z);
    pos -= size / 2;
    return (local_to_global * pos);
}

bool VoxelSource::Load(const std::string& filename) {
    std::ifstream input(filename);
    if (!Load(input, prob, dims)) {
        return (false);
    }
    std::partial_sum(prob.begin(), prob.end(), prob.begin());
    for (double& val : prob) {
        val /= prob.back();
    }
    return (true);
}

/*!
 * Reads a binary image.  Assumes the image has a 20 bytes, 4 int32 header that
 * contains:
 * - a "magic" number 65531, chosen arbitrarily
 * - a version number.  This only handles on version 1
 * - the dimensions of the image in x, y, and z
 * The remainder of the file is assumed to be x*y*z float32 values written in
 * [x][z][y] C order.
 */
bool VoxelSource::Load(
        std::istream& input,
        std::vector<double>& vox_vals,
        std::array<int,3>& dims)
{
    if (!input) {
        return (false);
    }
    int magic_number;
    int version_number;

    input.read(reinterpret_cast<char*>(&magic_number),
            sizeof(magic_number));
    input.read(reinterpret_cast<char*>(&version_number),
            sizeof(version_number));
    input.read(reinterpret_cast<char*>(dims.data()),
            sizeof(int) * dims.size());

    if (!input || (magic_number != 65531) || (version_number != 1)) {
        return (false);
    }

    int no_vox = dims[0] * dims[1] * dims[2];
    std::vector<float> data(no_vox);

    input.read(reinterpret_cast<char*>(data.data()), no_vox * sizeof(float));

    if (input.fail()) {
        return (false);
    }

    vox_vals.clear();
    vox_vals.resize(no_vox);
    int idx = 0;
    for (int x = 0; x < dims[0]; ++x) {
        for (int y = 0; y < dims[1]; ++y) {
            for (int z = 0; z < dims[2]; ++z) {
                // Calculate the index into the XZY array
                int data_idx = (x * dims[2] + z) * dims[1] + y;
                vox_vals[idx++] = data[data_idx];
            }
        }
    }
    return (true);
}

bool VoxelSource::Inside(const VectorR3&) const {
    // TODO: allow for positioning inside of voxelized sources
    return false;
}
