/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef AABB_H
#define AABB_H

#include <assert.h>
#include "Gray/VrMath/LinearR3.h"

class AABB
{
public:

    AABB() {}
    AABB( const VectorR3& boxMin, const VectorR3& boxMax );

    void Set( const VectorR3& boxMin, const VectorR3& boxMax );

    void SetNewAxisMin ( int axisNum, double newMin );	// Use 0, 1, 2, for x, y, z axes
    void SetNewAxisMax ( int axisNum, double newMax );

    const VectorR3& GetBoxMin() const
    {
        return bounds[0];
    }
    const VectorR3& GetBoxMax() const
    {
        return bounds[1];
    }

    VectorR3& GetBoxMin()
    {
        return bounds[0];
    }
    VectorR3& GetBoxMax()
    {
        return bounds[1];
    }

    double GetMinX() const
    {
        return GetBoxMin().x;
    }
    double GetMaxX() const
    {
        return GetBoxMax().x;
    }
    double GetMinY() const
    {
        return GetBoxMin().y;
    }
    double GetMaxY() const
    {
        return GetBoxMax().y;
    }
    double GetMinZ() const
    {
        return GetBoxMin().z;
    }
    double GetMaxZ() const
    {
        return GetBoxMax().z;
    }

    bool IsFlatX() const
    {
        return (GetBoxMin().x==GetBoxMax().x);
    }
    bool IsFlatY() const
    {
        return (GetBoxMin().y==GetBoxMax().y);
    }
    bool IsFlatZ() const
    {
        return (GetBoxMin().z==GetBoxMax().z);
    }

    bool WellFormed() const;	// If has non-negative volume
    bool IsEmpty() const;		// If has negative volume (definitely misformed in this case)

    // Update the Aabb to include the "newAabb"
    void EnlargeToEnclose ( const AABB& newAabb );

    // Miscellaneous functions
    double SurfaceArea() const;

    // Intersection functions
    // Form the intersection of two AABB's.
    // "this" is updating by intersecting it with aabb1.
    // Use IsEmpty to check if result has is non-empty.
    void IntersectAgainst( const AABB& aabb1 );

    bool RayIntersect(const VectorR3& startPos, const VectorR3& dir,
                      VectorR3 & dirInv, int & signDirX, int & signDirY,
                      int & signDirZ, double t0, double t1, double & tmin,
                      double & tmax) const;
    bool RayIntersect(const VectorR3& startPos, const VectorR3 & dirInv,
                      int signDirX, int signDirY, int signDirZ,
                      double t0, double t1, double & tmin,
                      double & tmax) const;
    bool Inside(const VectorR3 & pos);
private:
    VectorR3 bounds[2];
};

inline AABB::AABB( const VectorR3& boxMin, const VectorR3& boxMax )
{
    Set(boxMin, boxMax);
}

inline void AABB::Set( const VectorR3& boxMin, const VectorR3& boxMax )
{
    GetBoxMin() = boxMin;
    GetBoxMax() = boxMax;
    assert( WellFormed() );

}

// Use 0, 1, 2, for x, y, z axes
inline void AABB::SetNewAxisMin ( int axisNum, double newMin )
{
    switch (axisNum) {
    case 0:
        GetBoxMin().x = newMin;
        break;
    case 1:
        GetBoxMin().y = newMin;
        break;
    case 2:
        GetBoxMin().z = newMin;
        break;
    }
}

// Use 0, 1, 2, for x, y, z axes
inline void AABB::SetNewAxisMax ( int axisNum, double newMax )
{
    switch (axisNum) {
    case 0:
        GetBoxMax().x = newMax;
        break;
    case 1:
        GetBoxMax().y = newMax;
        break;
    case 2:
        GetBoxMax().z = newMax;
        break;
    }
}

// Form the intersection of two AABB's. Result is aabb2.
// Use IsEmpty to check if result has is non-empty.
inline void AABB::IntersectAgainst( const AABB& aabb1 )
{
    VectorR3 & a_min = GetBoxMin();
    VectorR3 & a_max = GetBoxMax();
    const VectorR3 & b_min = aabb1.GetBoxMin();
    const VectorR3 & b_max = aabb1.GetBoxMax();
    a_min.x = std::max(a_min.x, b_min.x);
    a_min.y = std::max(a_min.y, b_min.y);
    a_min.z = std::max(a_min.z, b_min.z);
    a_max.x = std::min(a_max.x, b_max.x);
    a_max.y = std::min(a_max.y, b_max.y);
    a_max.z = std::min(a_max.z, b_max.z);
}

inline bool AABB::WellFormed() const
{
    return (!IsEmpty());
}

// Flat boxes do not count as "empty"
inline bool AABB::IsEmpty() const
{
    return ((GetBoxMax().x < GetBoxMin().x) || (GetBoxMax().y < GetBoxMin().y) ||
            (GetBoxMax().z < GetBoxMin().z));
}

#endif
