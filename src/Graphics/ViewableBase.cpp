/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Graphics/ViewableBase.h"

void ViewableBase::CalcAABB( AABB& retAABB ) const
{
    VectorR3& theMin = retAABB.GetBoxMin();
    VectorR3& theMax = retAABB.GetBoxMax();

    VectorR3 dirVec;
    dirVec.SetUnitX();
    CalcBoundingPlanes( dirVec, &theMin.x, &theMax.x );
    dirVec.SetUnitY();
    CalcBoundingPlanes( dirVec, &theMin.y, &theMax.y );
    dirVec.SetUnitZ();
    CalcBoundingPlanes( dirVec, &theMin.z, &theMax.z );
}

bool ViewableBase::CalcExtentsInBox( const AABB& aabb, AABB& retAABB ) const
{
    CalcAABB( retAABB );
    retAABB.IntersectAgainst( aabb );
    return( !retAABB.IsEmpty() );
}
