/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <math.h>
#include "Gray/Graphics/ViewableEllipsoid.h"
#include "Gray/Graphics/ViewableSphere.h"
#include "Gray/VrMath/PolynomialRC.h"

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableEllipsoid::FindIntersectionNT (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    VectorR3 v = viewPos;
    v -= Center;
    double pdotuA = v^AxisA;
    double pdotuB = v^AxisB;
    double pdotuC = v^AxisC;
    double udotuA = viewDir^AxisA;
    double udotuB = viewDir^AxisB;
    double udotuC = viewDir^AxisC;

    double C = Square(pdotuA) + Square(pdotuB) + Square(pdotuC) - 1.0;
    double B = ( pdotuA*udotuA + pdotuB*udotuB + pdotuC*udotuC );
    if ( C>0.0 && B>=0.0 ) {
        return false;			// Pointing away from the ellipsoid
    }

    B += B;		// Double B to get final factor of 2.
    double A = Square(udotuA) + Square(udotuB) + Square(udotuC);

    double alpha1, alpha2;
    int numRoots = QuadraticSolveRealSafe( A, B, C, &alpha1, &alpha2 );
    if ( numRoots==0 ) {
        return false;
    }
    if ( alpha1>0.0 ) {
        if ( alpha1>=maxDistance ) {
            return false;				// Too far away
        }
        // Found an intersection from outside.
        returnedPoint.SetFrontFace();
        returnedPoint.SetMaterial(GetMaterialInner());
        *intersectDistance = alpha1;
    } else if ( numRoots==2 && alpha2>0.0 && alpha2<maxDistance ) {
        // Found an intersection from inside.
        returnedPoint.SetBackFace();
        returnedPoint.SetMaterial(GetMaterialOuter());
        *intersectDistance = alpha2;
    } else {
        return false;	// Both intersections behind us (should never get here)
    }

    // Calculate intersection position
    v=viewDir;
    v *= (*intersectDistance);
    v += viewPos;
    returnedPoint.SetPosition( v );	// Intersection Position

    v -= Center;	// Now v is the relative position
    double vdotuA = v^AxisA;
    double vdotuB = v^AxisB;
    double vdotuC = v^AxisC;
    v = vdotuA*AxisA + vdotuB*AxisB + vdotuC*AxisC;
    v.Normalize();

    return true;
}

void ViewableEllipsoid::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
    double centerDot = (u^Center);
    double deltaDot = sqrt(Square(RadiusA*RadiusA*(u^AxisA))
                           +Square(RadiusB*RadiusB*(u^AxisB))
                           +Square(RadiusC*RadiusC*(u^AxisC)));
    *maxDot = centerDot + deltaDot;
    *minDot = centerDot - deltaDot;
}
