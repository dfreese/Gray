/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

#include <Graphics/ViewableParallelogram.h>
#include <VrMath/Aabb.h>
#include <VrMath/PolygonClip.h>

// Precalculations for intersection testing speed
void ViewableParallelogram::PreCalcInfo()
{
    VectorR3 EdgeAB = VertexB - VertexA;
    VectorR3 EdgeBC = VertexC - VertexB;
    LengthAB = EdgeAB.Norm();
    LengthBC = EdgeBC.Norm();

    VertexD = VertexA;
    VertexD += EdgeBC;			// The fourth vertex

    EdgeAB /= LengthAB;					// Normalize
    EdgeBC /= LengthBC;					// Normalize

    Normal = EdgeAB*EdgeBC;
    Normal.Normalize();
    PlaneCoef = VertexB^Normal;

    NormalAB = Normal*EdgeAB;		// Normal in from edge AB
    NormalBC = Normal*EdgeBC;		// Normal in from edge BC
    NormalAB.ReNormalize();			// Just in case
    NormalBC.ReNormalize();

    CoefAB = VertexA^NormalAB;		// Coef. for edge AB.
    CoefBC = VertexB^NormalBC;
    CoefCD = VertexC^NormalAB;
    CoefDA = VertexA^NormalBC;
}

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableParallelogram::FindIntersectionNT (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    double mdotn = (viewDir^Normal);
    double planarDist = (viewPos^Normal)-PlaneCoef;

    // hit distance = -planarDist/mdotn
    if ( mdotn<=0.0 ) {
        if ( planarDist<=0 || planarDist >= -maxDistance*mdotn ) {
            return false;
        }
        returnedPoint.SetFrontFace();
    } else {
        if ( BackFaceCulled() || planarDist>=0 || -planarDist >= maxDistance*mdotn ) {
            return false;
        }
        returnedPoint.SetBackFace();
    }

    *intersectDistance = -planarDist/mdotn;
    VectorR3 v;
    v = viewDir;
    v *= *intersectDistance;
    v += viewPos;				// Point of view line intersecting plane

    double dotABnormal = v^NormalAB;
    if ( dotABnormal<CoefAB || dotABnormal>CoefCD ) {
        return false;
    }
    double dotBCnormal = v^NormalBC;
    if ( dotBCnormal<CoefBC || dotBCnormal>CoefDA ) {
        return false;
    }

    // Front/Back face info already set above
    returnedPoint.SetPosition( v );
    returnedPoint.SetMaterial( (mdotn<=0) ? *FrontMat : *BackMat );
    returnedPoint.SetNormal( Normal );

    // Compute the u-v coordinates
    double uCoord = (dotBCnormal-CoefBC)/(CoefDA-CoefBC);
    double vCoord = (dotABnormal-CoefAB)/(CoefCD-CoefAB);
    returnedPoint.SetUV(uCoord, vCoord);
    returnedPoint.SetFaceNumber( 0 );

    return true;
}

bool ViewableParallelogram::CalcPartials( const VisiblePoint& visPoint,
        VectorR3& retPartialU, VectorR3& retPartialV ) const
{
    retPartialU = VertexB;
    retPartialU -= VertexA;
    retPartialV = VertexD;
    retPartialV -= VertexA;
    return true;			// Not a singularity point (parallelograms should not be degenerate)
}

void ViewableParallelogram::CalcBoundingPlanes( const VectorR3& u,
        double *minDot, double *maxDot ) const
{
    double startdot = (u^VertexB);
    double mind = startdot;
    double maxd = mind;
    double t;
    t = (u^VertexC)-startdot;
    if ( t<0 ) {
        mind += t;
    } else {
        maxd +=t;
    }
    t = (u^VertexA)-startdot;
    if ( t<0 ) {
        mind += t;
    } else {
        maxd += t;
    }
    *minDot = mind;
    *maxDot = maxd;
}

bool ViewableParallelogram::CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const
{
    VectorR3 VertArray[60];
    VertArray[0] = GetVertexA();
    VertArray[1] = GetVertexB();
    VertArray[2] = GetVertexC();
    VertArray[3] = GetVertexD();

    const VectorR3 & boxBoundMin = boundingAABB.GetBoxMin();
    const VectorR3 & boxBoundMax = boundingAABB.GetBoxMax();
    int numClippedVerts = ClipConvexPolygonAgainstBoundingBox(4, VertArray,
                                                              GetNormal(),
                                                              boxBoundMin,
                                                              boxBoundMax );
    if ( numClippedVerts == 0 ) {
        return (false);
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
    return (true);
}
