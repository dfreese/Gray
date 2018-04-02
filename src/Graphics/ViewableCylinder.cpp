/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/VrMath/MathMisc.h"
#include "Gray/VrMath/PolynomialRC.h"
#include "Gray/Graphics/ViewableCylinder.h"

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableCylinder::FindIntersectionNT (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    double maxFrontDist = -DBL_MAX;
    double minBackDist = DBL_MAX;

    // Start with the bounding planes
    if ( IsRightCylinder() ) {
        double pdotn = (viewPos^CenterAxis)-CenterDotAxis;
        double udotn = viewDir^CenterAxis;
        if ( pdotn > HalfHeight ) {
            if ( udotn>=0.0 ) {
                return false;		// Above top plane pointing up
            }
            // Hits top from above
            maxFrontDist = (HalfHeight-pdotn)/udotn;
            minBackDist = -(HalfHeight+pdotn)/udotn;
        } else if ( pdotn < -HalfHeight ) {
            if ( udotn<=0.0 ) {
                return false;		// Below bottom, pointing down
            }
            // Hits bottom plane from below
            maxFrontDist = -(HalfHeight+pdotn)/udotn;
            minBackDist = (HalfHeight-pdotn)/udotn;
        } else if ( udotn<0.0 ) { // Inside, pointing down
            minBackDist = -(HalfHeight+pdotn)/udotn;
        } else if ( udotn>0.0 ) {		// Inside, pointing up
            minBackDist = (HalfHeight-pdotn)/udotn;
        }
    } else {
        // Has two bounding planes (not right cylinder)
        // First handle the top plane
        double pdotnCap = TopNormal^viewPos;
        double udotnCap = TopNormal^viewDir;
        if ( pdotnCap>TopPlaneCoef ) {
            if ( udotnCap>=0.0 ) {
                return false;		// Above top plane, pointing up
            }
            maxFrontDist = (TopPlaneCoef-pdotnCap)/udotnCap;
        } else if ( pdotnCap<TopPlaneCoef ) {
            if ( udotnCap>0.0 ) {
                // Below top plane, pointing up
                minBackDist = (TopPlaneCoef-pdotnCap)/udotnCap;
            }
        }
        // Second, handle the bottom plane
        pdotnCap = BottomNormal^viewPos;
        udotnCap = BottomNormal^viewDir;
        if ( pdotnCap<BottomPlaneCoef ) {
            if ( udotnCap>0.0 ) {
                double newBackDist = (BottomPlaneCoef-pdotnCap)/udotnCap;
                if ( newBackDist<maxFrontDist ) {
                    return false;
                }
                if ( newBackDist<minBackDist ) {
                    minBackDist = newBackDist;
                }
            }
        } else if ( pdotnCap>BottomPlaneCoef ) {
            if ( udotnCap>=0.0 ) {
                return false;		// Above bottom plane, pointing up (away)
            }
            // Above bottom plane, pointing down
            double newFrontDist = (BottomPlaneCoef-pdotnCap)/udotnCap;
            if ( newFrontDist>minBackDist ) {
                return false;
            }
            if ( newFrontDist>maxFrontDist ) {
                maxFrontDist = newFrontDist;
            }
        }
    }
    if ( maxFrontDist>maxDistance ) {
        return false;
    }

    // Now handle the cylinder sides
    double pdotuA = viewPos ^ AxisA;
    double pdotuB = viewPos ^ AxisB;
    double udotuA = viewDir ^ AxisA;
    double udotuB = viewDir ^ AxisB;

    double C = pdotuA*pdotuA + pdotuB*pdotuB - 1.0;
    double B = (pdotuA*udotuA + pdotuB*udotuB);

    if ( C>=0.0 && B>0.0 ) {
        return false;			// Pointing away from the cylinder
    }

    B += B;		// Double B for final 2.0 factor

    double A = udotuA*udotuA+udotuB*udotuB;

    double alpha1, alpha2;	// The roots, in order
    int numRoots = QuadraticSolveRealSafe(A, B, C, &alpha1, &alpha2);
    if ( numRoots==0 ) {
        return false;		// No intersection
    }
    if ( alpha1>maxFrontDist ) {
        if ( alpha1>minBackDist ) {
            return false;
        }
        maxFrontDist = alpha1;
    }
    if ( numRoots==2 && alpha2<minBackDist ) {
        if ( alpha2<maxFrontDist ) {
            return false;
        }
        minBackDist = alpha2;
    }

    // Put it all together:

    double alpha;
    if ( maxFrontDist>0.0 ) {
        returnedPoint.SetFrontFace();	// Hit from outside
        alpha = maxFrontDist;
    } else {
        returnedPoint.SetBackFace();	// Hit from inside
        alpha = minBackDist;
    }

    if ( alpha >= maxDistance ) {
        return false;
    }

    *intersectDistance = alpha;
    // Intersection point
    returnedPoint.SetPosition(viewDir * alpha + viewPos); 

    if (returnedPoint.IsFrontFacing()) {
        returnedPoint.SetMaterial(*GetMaterialOuter());
    } else {
        returnedPoint.SetMaterial(*GetMaterialInner());
    }

    return true;
}

void ViewableCylinder::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
    double centerDot = (u^Center);
    double AxisCdotU = (CenterAxis^u);
    if ( IsRightCylinderFlag ) {
        double deltaDot = HalfHeight*fabs(AxisCdotU)
                          + sqrt(Square(RadiusA*RadiusA*(AxisA^u))+Square(RadiusB*RadiusB*(AxisB^u)));
        *minDot = centerDot - deltaDot;
        *maxDot = centerDot + deltaDot;
        return;
    }

    double maxD, minD;
    // Handle top face
    VectorR3 perp = TopNormal;
    perp *= u;
    double alpha = (perp^AxisA)*RadiusA;
    double beta = (perp^AxisB)*RadiusB;
    if ( alpha==0.0 && beta==0.0 ) {	// If u perpindicular to top face
        maxD = minD = TopPlaneCoef*(u^TopNormal);
    } else {
        double solnX = -beta*RadiusA*RadiusA;
        double solnY = alpha*RadiusB*RadiusB;
        double ratio = sqrt( Square(alpha*RadiusB) + Square(beta*RadiusA) );
        solnX /= ratio;									// Now solnX and solnY give point on cylinder to check
        solnY /= ratio;
        VectorR3& trial = perp;					// Be careful: reuse of VectorR3 to avoid constructor overhead
        trial = Center;
        trial.AddScaled( AxisA, solnX*RadiusA );
        trial.AddScaled( AxisB, solnY*RadiusB );
        maxD = minD = (trial^u) + (TopPlaneCoef-(trial^TopNormal))*AxisCdotU/(CenterAxis^TopNormal);
        trial = Center;
        trial.AddScaled( AxisA, -solnX*RadiusA );
        trial.AddScaled( AxisB, -solnY*RadiusB );
        double newDot = (trial^u) + (TopPlaneCoef-(trial^TopNormal))*AxisCdotU/(CenterAxis^TopNormal);
        UpdateMinMax( newDot, minD, maxD );
    }

    // Handle bottom face
    perp = BottomNormal;
    perp *= u;
    alpha = (perp^AxisA)*RadiusA;
    beta = (perp^AxisB)*RadiusB;
    if ( alpha==0.0 && beta==0.0 ) {			// If u perpindicular to bottom face
        UpdateMinMax( BottomPlaneCoef*(u^BottomNormal), minD, maxD );
    } else {
        double solnX = -beta*RadiusA*RadiusA;
        double solnY = alpha*RadiusB*RadiusB;
        double ratio = sqrt( Square(alpha*RadiusB) + Square(beta*RadiusA) );
        solnX /= ratio;									// Now solnX and solnY give point on cylinder to check
        solnY /= ratio;
        VectorR3& trial = perp;					// Be careful: reuse of VectorR3 to avoid constructor overhead
        trial = Center;
        trial.AddScaled( AxisA, solnX*RadiusA );
        trial.AddScaled( AxisB, solnY*RadiusB );
        double newDot = (trial^u) + (BottomPlaneCoef-(trial^BottomNormal))*AxisCdotU/(CenterAxis^BottomNormal);
        UpdateMinMax( newDot, minD, maxD );
        trial = Center;
        trial.AddScaled( AxisA, -solnX*RadiusA );
        trial.AddScaled( AxisB, -solnY*RadiusB );
        newDot = (trial^u) + (BottomPlaneCoef-(trial^BottomNormal))*AxisCdotU/(CenterAxis^BottomNormal);
        UpdateMinMax( newDot, minD, maxD );
    }

    *minDot = minD;
    *maxDot = maxD;
}

