/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VOXELSOURCE_H
#define VOXELSOURCE_H

#include <array>
#include <iosfwd>
#include <vector>
#include "Gray/Sources/Source.h"
#include "Gray/VrMath/LinearR3.h"

class VoxelSource : public Source
{
public:
    VoxelSource(
            const VectorR3& position, const VectorR3& size,
            const VectorR3& axis, double activity);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;
    bool Load(const std::string& filename);
    static bool Load(
            std::istream& input,
            std::vector<double>& vox_vals,
            std::array<int,3>& dims);
private:
    std::array<int,3> dims;
    VectorR3 size;
    const RigidMapR3 local_to_global;
    const RigidMapR3 global_to_local;
    std::vector <double> prob;
};

#endif // VOXELSOURCE_H
