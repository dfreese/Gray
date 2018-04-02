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
#include "Gray/Graphics/ViewableCone.h"

ViewableCone::ViewableCone()
{
    Reset();
}

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableCone::FindIntersectionNT (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{

    double maxFrontDist = -DBL_MAX;
    double minBackDist = DBL_MAX;

    double viewPosdotCtr = viewPos^CenterAxis;
    double udotuCtr = viewDir^CenterAxis;
    if ( viewPosdotCtr>(ApexdotCenterAxis) && udotuCtr>=0.0 ) {
        return false;				// Above the cone's apex
    }

    // Start with the bounding base plane
    double pdotnCap = BaseNormal^viewPos;
    double udotnCap = BaseNormal^viewDir;
    if ( pdotnCap>BasePlaneCoef ) {
        if ( udotnCap>=0.0 ) {
            return false;		// Above (=outside) base plane, pointing away
        }
        maxFrontDist = (BasePlaneCoef-pdotnCap)/udotnCap;
    } else if ( pdotnCap<BasePlaneCoef ) {
        if ( udotnCap>0.0 ) {
            // Below (=inside) base plane, pointing towards the plane
            minBackDist = (BasePlaneCoef-pdotnCap)/udotnCap;
        }
    }

    // Now handle the cone's sides
    VectorR3 v = viewPos;
    v -= Apex;						// View position relative to the cone apex
    double pdotuCtr = v^CenterAxis;
    double pdotuA = v^AxisA;
    double pdotuB = v^AxisB;
    // udotuCtr already defined above
    double udotuA = viewDir^AxisA;
    double udotuB = viewDir^AxisB;

    double C = pdotuA*pdotuA + pdotuB*pdotuB - pdotuCtr*pdotuCtr;
    double B = 2.0*(pdotuA*udotuA + pdotuB*udotuB - pdotuCtr*udotuCtr);
    double A = udotuA*udotuA+udotuB*udotuB-udotuCtr*udotuCtr;

    double alpha1, alpha2;	// The roots, in order
    int numRoots = QuadraticSolveReal(A, B, C, &alpha1, &alpha2);
    if ( numRoots==0 ) {
        return false;		// No intersection
    }
    bool viewMoreVertical = ( A < 0.0 );
    if ( viewMoreVertical ) {
        // View line leaves and then enters the cone
        if ( alpha1<minBackDist && pdotuCtr+alpha1*udotuCtr<=0.0 ) {
            if ( alpha1<maxFrontDist ) {
                return false;
            }
            minBackDist = alpha1;
        } else if ( numRoots==2 && alpha2>maxFrontDist && pdotuCtr+alpha2*udotuCtr<=0.0 ) {
            if ( alpha2>minBackDist ) {
                return false;
            }
            maxFrontDist = alpha2;
        }
    } else {
        // view line enters and then leaves
        if ( alpha1>maxFrontDist ) {
            if ( pdotuCtr+alpha1*udotuCtr>0.0 ) {
                return false;				// Enters dual cone instead
            }
            if ( alpha1>minBackDist ) {
                return false;
            }
            maxFrontDist = alpha1;
        }
        if ( numRoots==2 && alpha2<minBackDist ) {
            if ( pdotuCtr+alpha2*udotuCtr>0.0 ) {
                return false;				// Is leaving dual cone instead
            }
            if ( alpha2<maxFrontDist ) {
                return false;
            }
            minBackDist = alpha2;
        }
    }

    // Put it all together:

    double alpha;
    if ( maxFrontDist>=0.0 ) {
        if ( maxFrontDist >= maxDistance ) {
            return false;
        }
        returnedPoint.SetFrontFace();	// Hit from outside
        alpha = maxFrontDist;
    } else {
        if ( minBackDist<0.0 || minBackDist >= maxDistance ) {
            return false;
        }
        alpha = minBackDist;
        returnedPoint.SetBackFace();	// Hit from inside
    }


    *intersectDistance = alpha;
    // Set v to the intersection point
    v = viewDir;
    v *= alpha;
    v += viewPos;
    returnedPoint.SetPosition( v );		// Intersection point
    if ( returnedPoint.IsFrontFacing() ) {
        returnedPoint.SetMaterial(GetMaterialOuter());
    } else {
        returnedPoint.SetMaterial(GetMaterialInner());
    }
    return true;
}

void ViewableCone::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
    // Find directions to the points that whose dot products need to be checked for min/max.
    VectorR3 perp = BaseNormal;
    perp *= u;								// Vector crossproduct
    double q1 = (perp^AxisA)*SlopeA;
    double q2 = (perp^AxisB)*SlopeB;
    double q3 = -(perp^CenterAxis);
    int numSolns;							// Number of points to check
    double soln1X, soln1Y;
    double soln2X, soln2Y;
    double a = Square(q1)-Square(q3*SlopeA);
    double b = q1*q2;
    double c = Square(q2)-Square(q3*SlopeB);
    if ( a==Square(q1) || c==Square(q2) ) {		// Catch explicitly the q3==0 case.
        numSolns = 1;							// In this case, there is only solution.
        if ( q1==0.0 && q2==0.0 ) {
            soln1X = 1.0;						// This is the case of all x,y are solutions.
            soln1Y = 0.0;						// We just choose (1,0) arbitrarily
        } else {
            soln1X = -q2;
            soln1Y = q1;
        }
    } else {
        numSolns = QuadraticSolveHomogeneousReal( a, b, c, &soln1X, &soln1Y, &soln2X, &soln2Y );
        if ( numSolns==3 ) {	// If u perpindicular to the base
            numSolns = 1;
            soln1X = 1.0;		// Pick some arbitrary solution to use
            soln1Y = 0.0;
        } else if ( numSolns==0 ) { // Roundoff errors have caused us to find no solution
            assert( b*b-a*c > -1.0e-13 );				// Determinant should be only barely non-zero!!
            numSolns = 1;								// soln1X and soln1Y have been set by QuadraticSolveHomogeneousReal.
            assert ( soln1X!=0.0 || soln1Y!=0.0 );		// Should never happen!
        }
        assert ( numSolns!= 0 );	// Should never happen!!  Plane thru origin must intersect the cone
    }

    // Dot product with apex point
    double apexDot = (u^Apex);
    double minD, maxD;
    minD = maxD = apexDot;

    // Try the two directions
    double apexDotBaseNormal = (Apex^BaseNormal);
    VectorR3& trial = perp;		// Reuse the perp vector (save a constructor)
    if ( numSolns==1 ) {
        double solnZ = sqrt( Square(soln1X*SlopeA)+Square(soln1Y*SlopeB) );
        trial = (soln1X/SlopeA)*AxisA+(soln1Y/SlopeB)*AxisB-solnZ*CenterAxis;
        double deltaDot = ((BasePlaneCoef-apexDotBaseNormal)/(BaseNormal^trial))*(trial^u);
        if ( deltaDot<0.0 ) {
            minD += deltaDot;
        } else {
            maxD += deltaDot;
        }
        trial += 2.0*solnZ*CenterAxis;
        deltaDot = ((BasePlaneCoef-apexDotBaseNormal)/(BaseNormal^trial))*(trial^u);
        UpdateMinMax( apexDot + deltaDot, minD, maxD );
    } else {						// if numSolns == 2
        // double solnZ = sqrt( Square(soln1X*SlopeA)+Square(soln1Y*SlopeB) );
        double solnZ = q1*soln1X + q2*soln1Y;
        trial = (q3*soln1X/SlopeA)*AxisA+(q3*soln1Y/SlopeB)*AxisB-solnZ*CenterAxis;			// q3 cannot be zero here
        double deltaDot = ((BasePlaneCoef-apexDotBaseNormal)/(BaseNormal^trial))*(trial^u);
        if ( deltaDot<0.0 ) {
            minD += deltaDot;
        } else {
            maxD += deltaDot;
        }
        solnZ = q1*soln2X + q2*soln2Y;
        trial = (q3*soln2X/SlopeA)*AxisA+(q3*soln2Y/SlopeB)*AxisB-solnZ*CenterAxis;
        deltaDot = ((BasePlaneCoef-apexDotBaseNormal)/(BaseNormal^trial))*(trial^u);
        UpdateMinMax( apexDot + deltaDot, minD, maxD );
    }
    *minDot = minD;
    *maxDot = maxD;
}
