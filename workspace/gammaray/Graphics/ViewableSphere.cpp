/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Graphics/ViewableSphere.h>
#include <VrMath/Aabb.h>
#include <VrMath/MathMisc.h>

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableSphere::FindIntersectionNT (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDist,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    VectorR3 tocenter(Center);
    tocenter -= viewPos;		// Vector view position to the center

    // D = Distance to pt on view line closest to sphere
    // v = vector from sphere center to the closest pt on view line
    // ASq = the distance from v to sphere center squared
    double D = (viewDir^tocenter);
    VectorR3 v(viewDir);
    v *= D;
    v -= tocenter;
    double ASq = v.NormSq();

    // Ray-line completely misses sphere, or just grazes it.
    if ( ASq >= RadiusSq ) {
        return false;
    }

    double BSq = RadiusSq-ASq;
    if ( D>0.0 && D*D>BSq &&
            (D<maxDist || BSq>Square(D-maxDist) ) ) {

        // Return the point where view intersects with the outside of
        //		the sphere.
        *intersectDistance = D-sqrt(BSq);
        v = viewDir;
        v *= *intersectDistance;
        v += viewPos;					//  Position of intersection
        returnedPoint.SetPosition( v );
        v -= Center;
        v /= Radius;	// Normalize: normal out from intersection pt
        v.ReNormalize();
        returnedPoint.SetNormal( v );
        returnedPoint.SetMaterial (*GetMaterialOuter());
        returnedPoint.SetFrontFace();	// Front face direction
        CalcUV( v, &(returnedPoint.GetUV()) );
        returnedPoint.SetFaceNumber( 0 );
        return true;
    }

    else if ( (D>0.0 || D*D<BSq) && D<maxDist && BSq<Square(D-maxDist) ) {

        // return the point where view exits the sphere
        *intersectDistance = D+sqrt(BSq);
        v = viewDir;
        v *= *intersectDistance;
        v += viewPos;
        returnedPoint.SetPosition( v );
        v -= Center;
        v /= Radius;	// Normalize, but still points outward
        v.ReNormalize(); // Just in case
        returnedPoint.SetNormal( v );
        returnedPoint.SetMaterial (*GetMaterialInner());
        returnedPoint.SetBackFace();
        CalcUV( v, &(returnedPoint.GetUV()) );
        return true;
    }

    else {
        return false;
    }
}

bool ViewableSphere::QuickIntersectTest(
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDist,
    double *intersectDistance,
    const VectorR3& centerPos, double radiusSq )
{
    VectorR3 tocenter(centerPos);
    tocenter -= viewPos;		// Vector view position to the center

    // D = Distance to pt on view line closest to sphere
    // v = vector from sphere center to the closest pt on view line
    // ASq = the distance from v to sphere center squared
    double D = (viewDir^tocenter);
    VectorR3 v(viewDir);
    v *= D;
    v -= tocenter;
    double ASq = v.NormSq();

    // Ray-line completely misses sphere, or just grazes it.
    if ( ASq >= radiusSq ) {
        return false;
    }

    double BSq = radiusSq-ASq;
    if ( D>0.0 && D*D>BSq && (D<maxDist || BSq>Square(D-maxDist) ) ) {
        // It hits the sphere as it enters.
        *intersectDistance = D-sqrt(BSq);
        return true;
    }

    else if ( (D>0.0 || D*D<BSq) && D<maxDist && BSq<Square(D-maxDist) ) {
        // it hits the sphere as it exits
        *intersectDistance = D+sqrt(BSq);
        return true;
    }

    else {
        return false;
    }
}


void ViewableSphere::CalcUV( const VectorR3& posVec, VectorR2* returnedUV ) const
{
    double z = posVec^AxisA;
    double y = posVec^AxisC;
    double x = posVec^AxisB;

    CalcUV( x, y, z, uvProjectionType, returnedUV );
}

void ViewableSphere::CalcUV( double x, double y, double z, int uvprojectiontype,
                             VectorR2* returnedUV )
{

    double u = atan2(x,z)*PI2inv + 0.5;
    double v;

    switch ( uvprojectiontype ) {

    case 0:		// Spherical projection
        if ( y>=1.0 ) {
            v = 1.0;				// avoid roundoff error exceptions
        } else if ( y<=-1.0 ) {
            v = 0.0;				// and again
        } else {
            v = 1.0-acos(y)*PIinv;	// We usually do this.
        }
        break;

    case 1:		// Cylindrical projection

        v = (y+1.0)*0.5;

        break;
    }

    returnedUV->Set(u, v);
}

void ViewableSphere::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
    double cd = (u^Center);
    *maxDot = cd+Radius;
    *minDot = cd-Radius;
}

bool ViewableSphere::CalcPartials( const VisiblePoint& visPoint,
                                   VectorR3& retPartialU, VectorR3& retPartialV ) const
{
    retPartialV = visPoint.GetPosition();
    retPartialV -= Center;
    retPartialU = retPartialV;
    retPartialU *= AxisC;			// Magnitude = R sin(phi).
    retPartialU *= -2 * M_PI;			// Adjust for [0,1] domain instead of [-pi,pi]

    retPartialV *= retPartialU;		// Pointing in right direction, magnitude 2 pi R^2 sin(phi)
    // Sign and magnitude adjusted below.
    double badMagSq = retPartialV.NormSq();
    double h;

    switch ( uvProjectionType ) {

    case 0:	// Spherical projection
        h = sqrt(badMagSq);
        if ( h==0.0 ) {
            retPartialV = AxisB;
            return false;
        }
        retPartialV *= (M_PI*Radius/h);  // Convert to domain [0,1] instead of [-pi/2,pi/2].
        // Compensate for sign and R^2 magnitude.
        break;

    case 1:
        h = 2.0*(visPoint.GetV()-0.5);	// In range [-1,1]
        h = sqrt(badMagSq*(1.0 - h*h));			// Derivative of arcsin(h) = 1/sqrt(1-h^2)
        if ( h==0 ) {
            retPartialV = AxisB;
            return false;
        }
        retPartialV *= M_PI*Radius/h;			// Adjust sign and magnitude
        break;

    }
    return true;
}

namespace {

// CalcMinMaxSquares
//  Input values: valMin and valMax where valMin <= valMax
//  Returns the values  Min{ x*x :  x \in [valMin, valMax] }
//                 and  Max{ x*x :  x \in [valMin, valMax] }
void CalcMinMaxSquares( double valMin, double valMax, double* valSqMin, double* valSqMax )
{
    assert ( valMin<=valMax );
    if ( valMin<0.0 && valMax>0.0 ) {
        *valSqMin = 0.0;
        *valSqMax = ( valMax > -valMin ) ? Square(valMax) : Square(valMin);
    } else {
        *valSqMin = Square(valMin);
        *valSqMax = Square(valMax);
        if ( (*valSqMin)>(*valSqMax) ) {
            double temp = *valSqMin;
            *valSqMin = *valSqMax;
            *valSqMax = temp;
        }
    }
}

// CalcExtentsHelpForSphere
//    Helper routine for CalcExtentsInBox( ViewableSphere& ...)
//        Returns false if extent in box is empty.
inline bool CalcExtentsHelpForSphere( double boxMin, double boxMax,
                                     double radiusSq, double otherSqMin, double otherSqMax,
                                     double* minExtent, double* maxExtent )
{
    double maxSq = radiusSq - otherSqMin;
    double maxPos;                        // Max x value (always positive)
    if ( maxSq<0.0 ) {
        return false;
    } else {
        maxPos = sqrt(maxSq);
    }
    double minSq = radiusSq - otherSqMax;
    double minPos = (minSq > 0.0) ? sqrt(minSq) : 0.0;

    // minPos and maxPos are the min/max possible positive values
    //    I.e., permissible values are from [-maxPos,-minPos]\cup [minPos,maxPos].
    //    This now needs to be intersected with [boxMin, boxMax]
    if ( maxPos<=boxMin || (-maxPos)>=boxMax ) {
        return false;        // Test uses <= to ignore single points of intersectio
    }
    if ( boxMin<(-minPos) ) {
        *minExtent = std::max(boxMin,-maxPos);
        if ( boxMax>minPos ) {
            *maxExtent = std::min(boxMax,maxPos);
        } else {
            *maxExtent = std::min(boxMax,-minPos);
        }
        return true;
    } else if ( boxMax>minPos ) {
        *minExtent = std::max(boxMin,minPos);
        *maxExtent = std::min(boxMax,maxPos);
        return true;
    } else {
        return false;
    }
}

}

/*!
 * Zero area intersections are ignored, e.g. where the sphere is tangent to the
 * face of a cube or intersects a corner.
 */
bool ViewableSphere::CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const
{
    const VectorR3& center = GetCenter();

    // Get min and max relative to center of sphere
    const VectorR3 & boxBoundMin = boundingAABB.GetBoxMin();
    const VectorR3 & boxBoundMax = boundingAABB.GetBoxMax();
    VectorR3 min = boxBoundMin;
    min -= center;
    VectorR3 max = boxBoundMax;
    max -= center;

    double xSqMin, xSqMax;
    double ySqMin, ySqMax;
    double zSqMin, zSqMax;
    CalcMinMaxSquares( min.x, max.x, &xSqMin, &xSqMax );
    CalcMinMaxSquares( min.y, max.y, &ySqMin, &ySqMax );
    CalcMinMaxSquares( min.z, max.z, &zSqMin, &zSqMax );

    VectorR3 * extentsMin = &retAABB.GetBoxMin();
    VectorR3 * extentsMax = &retAABB.GetBoxMax();

    double radiusSq = Square(GetRadius());        // Square of the radius
    if ( !CalcExtentsHelpForSphere( min.x, max.x, radiusSq, ySqMin+zSqMin, ySqMax+zSqMax,
                                   &(extentsMin->x), &(extentsMax->x) ) ) {
        return false;
    }
    if ( !CalcExtentsHelpForSphere( min.y, max.y, radiusSq, xSqMin+zSqMin, xSqMax+zSqMax,
                                   &(extentsMin->y), &(extentsMax->y) ) ) {
        return false;
    }
    if ( !CalcExtentsHelpForSphere( min.z, max.z, radiusSq, xSqMin+ySqMin, xSqMax+ySqMax,
                                   &(extentsMin->z), &(extentsMax->z) ) ) {
        return false;
    }

    *extentsMin += center;
    *extentsMax += center;
    return true;
}
