/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/VrMath/Aabb.h"

// Update the Aabb to include the "newAabb"
void AABB::EnlargeToEnclose ( const AABB& aabbToEnclose )
{
    VectorR3 & a_min = GetBoxMin();
    VectorR3 & a_max = GetBoxMax();
    const VectorR3 & b_min = aabbToEnclose.GetBoxMin();
    const VectorR3 & b_max = aabbToEnclose.GetBoxMax();
    a_min.x = std::min(a_min.x, b_min.x);
    a_min.y = std::min(a_min.y, b_min.y);
    a_min.z = std::min(a_min.z, b_min.z);
    a_max.x = std::max(a_max.x, b_max.x);
    a_max.y = std::max(a_max.y, b_max.y);
    a_max.z = std::max(a_max.z, b_max.z);
}

double AABB::SurfaceArea() const
{
    VectorR3 delta = GetBoxMax() - GetBoxMin();
    return 2.0*(delta.x*delta.y + delta.x*delta.z + delta.y*delta.z);
}

bool AABB::RayIntersect(const VectorR3& startPos, const VectorR3& dir,
                        VectorR3 & dirInv, int & signDirX, int & signDirY,
                        int & signDirZ, double t0, double t1, double & tmin,
                        double & tmax) const
{
    dirInv.x = 1 / dir.x;
    dirInv.y = 1 / dir.y;
    dirInv.z = 1 / dir.z;
    signDirX = (dirInv.x < 0);
    signDirY = (dirInv.y < 0);
    signDirZ = (dirInv.z < 0);
    return(RayIntersect(startPos, dirInv, signDirX, signDirY, signDirZ,
                        t0, t1, tmin, tmax));
}

/*!
 * This implementation is careful about the IEEE spec for handling divide by
 * 0 cases.  For more details see this paper here:
 * http://people.csail.mit.edu/amy/papers/box-jgt.pdf
 */
bool AABB::RayIntersect(const VectorR3& startPos, const VectorR3 & dirInv,
                        int signDirX, int signDirY, int signDirZ,
                        double t0, double t1, double & tmin,
                        double & tmax) const
{
    tmin = (bounds[signDirX].x - startPos.x) * dirInv.x;
    tmax = (bounds[1-signDirX].x - startPos.x) * dirInv.x;
    float tymin = (bounds[signDirY].y - startPos.y) * dirInv.y;
    float tymax = (bounds[1-signDirY].y - startPos.y) * dirInv.y;
    if ((tmin > tymax) || (tymin > tmax)) {
        return(false);
    }
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (bounds[signDirZ].z - startPos.z) * dirInv.z;
    float tzmax = (bounds[1-signDirZ].z - startPos.z) * dirInv.z;
    if ((tmin > tzmax) || (tzmin > tmax)) {
        return(false);
    }

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;
    return((tmin < t1) && (tmax > t0));
}

bool AABB::Inside(const VectorR3 & pos) {
    return ((pos.x >= GetMinX()) &&
            (pos.x <= GetMaxX()) &&
            (pos.y >= GetMinY()) &&
            (pos.y <= GetMaxY()) &&
            (pos.z >= GetMinZ()) &&
            (pos.z <= GetMaxZ()));
}
