/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Graphics/ViewableParallelepiped.h"
#include "Gray/VrMath/Aabb.h"
#include "Gray/VrMath/PolygonClip.h"

void ViewableParallelepiped::CalcPlaneInfo()
{
    VectorR3 EdgeAB = VertexB;
    EdgeAB -= VertexA;
    VectorR3 EdgeAC = VertexC;
    EdgeAC -= VertexA;
    VectorR3 EdgeAD = VertexD;
    EdgeAD -= VertexA;

    // Front and back faces
    NormalABC = EdgeAB;
    NormalABC *= EdgeAC;
    NormalABC.Normalize();
    TopCoefABC = (NormalABC)^VertexA;		// Front face coef.
    BottomCoefABC = (NormalABC)^VertexD;	// Back face coef.
    if ( TopCoefABC<BottomCoefABC ) {
        double temp = TopCoefABC;
        TopCoefABC = BottomCoefABC;
        BottomCoefABC = temp;
    }

    // Top and bottom faces
    NormalABD = EdgeAB;
    NormalABD *= EdgeAD;
    NormalABD.Normalize();
    TopCoefABD = (NormalABD)^VertexC;		// Top face coef.
    BottomCoefABD = (NormalABD)^VertexA;	// Bottom face coef.
    if ( TopCoefABD<BottomCoefABD ) {
        double temp = TopCoefABD;
        TopCoefABD = BottomCoefABD;
        BottomCoefABD = temp;
    }

    // Left and right faces
    NormalACD = EdgeAD;
    NormalACD *= EdgeAC;
    NormalACD.Normalize();
    TopCoefACD = (NormalACD)^VertexB;		// Right face coef.
    BottomCoefACD = (NormalACD)^VertexA;	// Left face coef.
    if ( TopCoefACD<BottomCoefACD ) {
        double temp = TopCoefACD;
        TopCoefACD = BottomCoefACD;
        BottomCoefACD = temp;
    }

}


bool DoTwoPlanes( const VectorR3& viewPos, const VectorR3& viewDir,
                  const VectorR3 normal, double topCoef, double bottomCoef, int planeNum,
                  double *maxFront, int *frontNum, double *minBack, int *backNum,
                  double maxHitDistanceAllowed)
{
    double pdotn = (viewPos^normal);
    double udotn = (viewDir^normal);

    if ( udotn > 0.0 ) {
        if ( pdotn>topCoef ) {
            return false;		// Above top and pointing up
        }
        if ( bottomCoef-pdotn>=0.0 ) {
            double newFront = (bottomCoef-pdotn)/udotn;
            if ( newFront > *maxFront ) {
                if ( newFront > *minBack || newFront > maxHitDistanceAllowed ) {
                    return false;
                }
                *maxFront = newFront;		// Hits bottom from below (from outside)
                *frontNum = planeNum+1;		// +1 for bottom plane
            }
        }
        double newBack = (topCoef-pdotn)/udotn;
        if ( newBack < *minBack ) {
            if ( newBack < *maxFront ) {
                return false;
            }
            *minBack = newBack;					// Hits top from inside
            *backNum = planeNum;
        }
    } else if ( udotn < 0.0 ) {
        if ( pdotn<bottomCoef ) {
            return false;		// Below bottom and pointing down
        }
        if ( topCoef-pdotn<=0.0 ) {
            double newFront = (topCoef-pdotn)/udotn;
            if ( newFront > *maxFront ) {
                if ( newFront > *minBack || newFront > maxHitDistanceAllowed ) {
                    return false;
                }
                *maxFront = newFront;		// Hits top from above (from outside)
                *frontNum = planeNum;
            }
        }
        double newBack = (bottomCoef-pdotn)/udotn;
        if ( newBack < *minBack ) {
            if ( newBack < *maxFront ) {
                return false;
            }
            *minBack = newBack;					// Hits top from inside
            *backNum = planeNum+1;				// +1 for bottom plane
        }
    } else if ( pdotn<bottomCoef || pdotn>topCoef ) {
        return false;
    }
    return true;
}

bool DoTwoPlanesCheck(
    const VectorR3& viewPos, const VectorR3& viewDir,
    const VectorR3 normal, double topCoef, double bottomCoef, int planeNum,
    double *maxFront, int *frontNum, double *minBack, int *backNum,
    double maxHitDistanceAllowed)
{
    if ( bottomCoef<topCoef ) {
        return DoTwoPlanes( viewPos, viewDir, normal, topCoef, bottomCoef, planeNum,
                            maxFront, frontNum, minBack, backNum, maxHitDistanceAllowed);
    }

    double pdotn = (viewPos^normal);
    double udotn = (viewDir^normal);
    double hitDist = (topCoef-pdotn)/udotn;
    if ( hitDist<0.0 || hitDist>maxHitDistanceAllowed
            || hitDist<*maxFront || hitDist>*minBack ) {
        return false;
    }
    *maxFront = hitDist;
    *minBack = hitDist;
    return true;
}

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableParallelepiped::FindIntersectionNT (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    double maxFrontDist = -DBL_MAX;
    int frontFaceNum;
    double minBackDist = DBL_MAX;
    int backFaceNum;
    // Face nums: 0,1,2,3,4,5 = front, back, top, bottom, right, left.

    if (!DoTwoPlanes( viewPos, viewDir, NormalABC, TopCoefABC, BottomCoefABC, 0,
                      &maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
        return false;
    }
    if (!DoTwoPlanes( viewPos, viewDir, NormalABD, TopCoefABD, BottomCoefABD, 2,
                      &maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
        return false;
    }
    if (!DoTwoPlanes( viewPos, viewDir, NormalACD, TopCoefACD, BottomCoefACD,4,
                      &maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
        return false;
    }

    double alpha;
    if ( maxFrontDist>0.0 ) {
        alpha = maxFrontDist;
        returnedPoint.SetFrontFace();
        returnedPoint.SetMaterial(ViewableBase::GetMaterialBack());
    } else if ( minBackDist>0.0 && minBackDist<maxDistance ) {
        alpha = minBackDist;
        returnedPoint.SetBackFace();
        returnedPoint.SetMaterial(ViewableBase::GetMaterialFront());
    } else {
        return false;
    }

    VectorR3 v = viewDir;
    v *= alpha;
    v += viewPos;		// Intersection point
    *intersectDistance = alpha;
    returnedPoint.SetPosition( v );

    return true;
}

void ViewableParallelepiped::CalcBoundingPlanes( const VectorR3& u,
        double *minDot, double *maxDot ) const
{
    double startdot = (u^VertexA);
    double mind = startdot;
    double maxd = mind;
    double t;
    t = (u^VertexB)-startdot;
    if ( t<0 ) {
        mind += t;
    } else {
        maxd +=t;
    }
    t = (u^VertexC)-startdot;
    if ( t<0 ) {
        mind += t;
    } else {
        maxd += t;
    }
    t = (u^VertexD)-startdot;
    if ( t<0 ) {
        mind += t;
    } else {
        maxd += t;
    }
    *minDot = mind;
    *maxDot = maxd;
}

bool ViewableParallelepiped::CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const
{
    const VectorR3 boxBoundMin = boundingAABB.GetBoxMin();
    const VectorR3 boxBoundMax = boundingAABB.GetBoxMax();
    VectorR3 VertArray[60];
    VectorR3 deltaAB = GetVertexB();
    VectorR3 deltaAC = GetVertexC();
    VectorR3 deltaAD = GetVertexD();
    const VectorR3& vertexA = GetVertexA();
    deltaAB -= vertexA;
    deltaAC -= vertexA;
    deltaAD -= vertexA;

    int baseCount = 0;

    // Front face
    VertArray[baseCount+0] = GetVertexA();
    VertArray[baseCount+1] = GetVertexB();
    VertArray[baseCount+2] = GetVertexC() + deltaAB;
    VertArray[baseCount+3] = GetVertexC();
    baseCount += ClipConvexPolygonAgainstBoundingBox(4, &(VertArray[baseCount]),
                                                     GetNormalABC(),
                                                     boxBoundMin, boxBoundMax);
    // Back face
    VertArray[baseCount+0] = GetVertexA() + deltaAD;
    VertArray[baseCount+1] = GetVertexB() + deltaAD;
    VertArray[baseCount+2] = GetVertexC() + deltaAB + deltaAD;
    VertArray[baseCount+3] = GetVertexC() + deltaAD;
    baseCount += ClipConvexPolygonAgainstBoundingBox(4, &(VertArray[baseCount]),
                                                     GetNormalABC(),
                                                     boxBoundMin, boxBoundMax);
    // Left face
    VertArray[baseCount+0] = GetVertexA();
    VertArray[baseCount+1] = GetVertexC();
    VertArray[baseCount+2] = GetVertexD() + deltaAC;
    VertArray[baseCount+3] = GetVertexD();
    baseCount += ClipConvexPolygonAgainstBoundingBox(4, &(VertArray[baseCount]),
                                                     GetNormalABC(),
                                                     boxBoundMin, boxBoundMax);
    // Right face
    VertArray[baseCount+0] = GetVertexA() + deltaAB;
    VertArray[baseCount+1] = GetVertexC() + deltaAB;
    VertArray[baseCount+2] = GetVertexD() + deltaAC + deltaAB;
    VertArray[baseCount+3] = GetVertexD() + deltaAB;
    baseCount += ClipConvexPolygonAgainstBoundingBox(4, &(VertArray[baseCount]),
                                                     GetNormalABC(),
                                                     boxBoundMin, boxBoundMax);
    // Bottom face
    VertArray[baseCount+0] = GetVertexA();
    VertArray[baseCount+1] = GetVertexB();
    VertArray[baseCount+2] = GetVertexD() + deltaAB;
    VertArray[baseCount+3] = GetVertexD();
    baseCount += ClipConvexPolygonAgainstBoundingBox(4, &(VertArray[baseCount]),
                                                     GetNormalABC(),
                                                     boxBoundMin, boxBoundMax);
    // Top face
    VertArray[baseCount+0] = GetVertexA() + deltaAC;
    VertArray[baseCount+1] = GetVertexB() + deltaAC;
    VertArray[baseCount+2] = GetVertexD() + deltaAB + deltaAC;
    VertArray[baseCount+3] = GetVertexD() + deltaAC;
    baseCount += ClipConvexPolygonAgainstBoundingBox(4, &(VertArray[baseCount]),
                                                     GetNormalABC(),
                                                     boxBoundMin, boxBoundMax);

    int numClippedVerts = baseCount;
    if ( numClippedVerts == 0 ) {
        return false;
    }
    VectorR3 * extentsMin = &retAABB.GetBoxMin();
    VectorR3 * extentsMax = &retAABB.GetBoxMax();
    CalcBoundingBox(numClippedVerts, VertArray, extentsMin, extentsMax);

    // Next six lines to avoid roundoff errors putting extents outside the bounding box
    ClampRange( &extentsMin->x, boxBoundMin.x, boxBoundMax.x );
    ClampRange( &extentsMin->y, boxBoundMin.y, boxBoundMax.y );
    ClampRange( &extentsMin->z, boxBoundMin.z, boxBoundMax.z );
    ClampRange( &extentsMax->x, boxBoundMin.x, boxBoundMax.x );
    ClampRange( &extentsMax->y, boxBoundMin.y, boxBoundMax.y );
    ClampRange( &extentsMax->z, boxBoundMin.z, boxBoundMax.z );
    return true;
}

bool ViewableParallelepiped::QuickIntersectTest(
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistanceIn, double *intersectDistanceOut,
    const VectorR3& NormalA, double MinDotA, double MaxDotA,
    const VectorR3& NormalB, double MinDotB, double MaxDotB,
    const VectorR3& NormalC, double MinDotC, double MaxDotC )
{
    double maxFrontDist = -DBL_MAX;
    int frontFaceNum;
    double minBackDist = DBL_MAX;
    int backFaceNum;
    // Face nums: 0,1,2,3,4,5 = A-top, A-bottom, B-top, B-bottom, C-top, C-bottom.

    if (!DoTwoPlanesCheck( viewPos, viewDir, NormalA, MaxDotA, MinDotA, 0,
                           &maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
        return false;
    }
    if (!DoTwoPlanesCheck( viewPos, viewDir, NormalB, MaxDotB, MinDotB, 2,
                           &maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
        return false;
    }
    if (!DoTwoPlanesCheck( viewPos, viewDir, NormalC, MaxDotC, MinDotC, 4,
                           &maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
        return false;
    }

    *intersectDistanceIn = maxFrontDist;
    *intersectDistanceOut = minBackDist;
    assert( minBackDist>=0.0 );
    return true;
}

bool ViewableParallelepiped::QuickIntersectTest(
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance,
    const VectorR3& NormalA, double MinDotA, double MaxDotA,
    const VectorR3& NormalB, double MinDotB, double MaxDotB,
    const VectorR3& NormalC, double MinDotC, double MaxDotC )
{
    double temp;
    return QuickIntersectTest( viewPos, viewDir, maxDistance,
                               intersectDistance, &temp,
                               NormalA, MinDotA, MaxDotA,
                               NormalB, MinDotB, MaxDotB,
                               NormalC, MinDotC, MaxDotC );
}
