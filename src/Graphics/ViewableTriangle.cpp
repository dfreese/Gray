/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Graphics/ViewableTriangle.h"
#include "Gray/VrMath/Aabb.h"
#include "Gray/VrMath/PolygonClip.h"

void ViewableTriangle::PreCalcInfo()
{
    VectorR3 EdgeAB = VertexB - VertexA;
    VectorR3 EdgeBC = VertexC - VertexB;
    VectorR3 EdgeCA = VertexA - VertexC;

    if ( (EdgeAB^EdgeBC) < (EdgeBC^EdgeCA) ) {
        Normal = EdgeAB*EdgeBC;
    } else {
        Normal = EdgeBC*EdgeCA;
    }
    Normal.Normalize();		// Unit vector to triangle's plane

    PlaneCoef = (Normal^VertexA);	// Same coef for all three vertices.

    double A = EdgeAB.NormSq();
    double B = (EdgeAB^EdgeCA);
    double C = EdgeCA.NormSq();
    double Dinv = 1.0/(A*C-B*B);
    A *= Dinv;
    B *= Dinv;
    C *= Dinv;
    Ubeta = EdgeAB;
    Ubeta *= C;
    Ubeta.AddScaled( EdgeCA, -B );
    Ugamma = EdgeCA;
    Ugamma *= -A;
    Ugamma.AddScaled( EdgeAB, B );
}


// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableTriangle::FindIntersectionNT (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    double mdotn = (viewDir^Normal);
    double planarDist = (viewPos^Normal)-PlaneCoef;

    // hit distance = -planarDist/mdotn
    bool frontFace = (mdotn<=0.0);
    if ( frontFace ) {
        if ( planarDist<=0 || planarDist >= -maxDistance*mdotn ) {
            return false;
        }
    } else {
        if ( BackFaceCulled() || planarDist>=0 || -planarDist >= maxDistance*mdotn ) {
            return false;
        }
    }

    *intersectDistance = -planarDist/mdotn;
    VectorR3 q;
    q = viewDir;
    q *= *intersectDistance;
    q += viewPos;						// Point of view line intersecting plane

    // Compute barycentric coordinates
    VectorR3 v(q);
    v -= VertexA;
    double vCoord = (v^Ubeta);
    if ( vCoord<0.0 ) {
        return false;
    }
    double wCoord = (v^Ugamma);
    if ( wCoord<0.0 || vCoord+wCoord>1.0 ) {
        return false;
    }

    returnedPoint.SetPosition( q );		// Set point of intersection
    returnedPoint.SetUV( vCoord, wCoord );

    // Front/Back face info already set above
    if ( frontFace ) {
        returnedPoint.SetMaterial(*ViewableBase::GetMaterialFront());
        returnedPoint.SetFrontFace();
    } else {
        returnedPoint.SetMaterial(*ViewableBase::GetMaterialBack());
        returnedPoint.SetBackFace();
    }
    returnedPoint.SetNormal( Normal );
    returnedPoint.SetFaceNumber( 0 );

    return true;
}

void ViewableTriangle::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
    double mind = (u^VertexA);
    double maxd = (u^VertexB);
    double t;
    if ( maxd < mind ) {
        t = maxd;				// Swap values, so mind < maxd
        maxd = mind;
        mind = t;
    }
    t = (u^VertexC);
    if ( t<mind ) {
        mind = t;
    } else if ( t>maxd ) {
        maxd = t;
    }
    *minDot = mind;
    *maxDot = maxd;
}

bool ViewableTriangle::CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const
{
    VectorR3 VertArray[60];
    const VectorR3 & boxBoundMin = boundingAABB.GetBoxMin();
    const VectorR3 & boxBoundMax = boundingAABB.GetBoxMax();
    VertArray[0] = GetVertexA();
    VertArray[1] = GetVertexB();
    VertArray[2] = GetVertexC();

    int numClippedVerts = ClipConvexPolygonAgainstBoundingBox(3, VertArray,
                                                              GetNormal(),
                                                              boxBoundMin,
                                                              boxBoundMax );
    if ( numClippedVerts == 0 ) {
        return false;
    }
    VectorR3 * extentsMin = &retAABB.GetBoxMin();
    VectorR3 * extentsMax = &retAABB.GetBoxMax();
    CalcBoundingBox( numClippedVerts, VertArray, extentsMin, extentsMax );

    // Next six lines to avoid roundoff errors putting extents outside the bounding box
    ClampRange( &extentsMin->x, boxBoundMin.x, boxBoundMax.x );
    ClampRange( &extentsMin->y, boxBoundMin.y, boxBoundMax.y );
    ClampRange( &extentsMin->z, boxBoundMin.z, boxBoundMax.z );
    ClampRange( &extentsMax->x, boxBoundMin.x, boxBoundMax.x );
    ClampRange( &extentsMax->y, boxBoundMin.y, boxBoundMax.y );
    ClampRange( &extentsMax->z, boxBoundMin.z, boxBoundMax.z );
    return true;
}

bool ViewableTriangle::CalcPartials( const VisiblePoint& visPoint,
                                     VectorR3& retPartialU, VectorR3& retPartialV ) const
{
    retPartialU = VertexB;
    retPartialU -= VertexA;
    retPartialV = VertexC;
    retPartialV -= VertexA;
    return true;			// Not a singularity point (triangles should not be degenerate)
}
