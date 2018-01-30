/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VOXELSOURCE_H
#define VOXELSOURCE_H

#include <Sources/Source.h>
#include <vector>

class VoxelSource : public Source
{
public:
    ~VoxelSource();
    VoxelSource();
    VoxelSource(const VectorR3 &p, int dims[3], const VectorR3 & vox_scale, double act);
    bool Load(const char * filename);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;
private:
    int dimension[3];
    VectorR3 scale;
    size_t search(double e, size_t b_idx, size_t s_idx) const;
    int AddSource(double val);

    std::vector <double> prob;
    std::vector <int*> index;
};

#endif // VOXELSOURCE_H
