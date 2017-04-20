/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
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

#ifndef VIEWABLETRIANGLE_H
#define VIEWABLETRIANGLE_H

#include <Graphics/ViewableBase.h>
#include <VrMath/LinearR2.h>

class ViewableTriangle : public ViewableBase
{

public:
    ViewableTriangle();

    // Three vertices in counter-clockwise order.
    void Init( const double* vertexpositions);
    void Init( const float* vertexpositions);
    void Init( const VectorR3& vA, const VectorR3& vB, const VectorR3& vC );

    bool IsTwoSided() const
    {
        return (ViewableBase::GetMaterialBack() != 0);
    }
    bool BackFaceCulled() const
    {
        return (ViewableBase::GetMaterialBack() == 0);
    }

    // Returns an intersection if found with distance maxDistance
    // viewDir must be a unit vector.
    // intersectDistance and visPoint are returned values.
    virtual bool FindIntersectionNT (
        const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
        double *intersectDistance, VisiblePoint& returnedPoint ) const;
    void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const;
    bool CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const;
    bool CalcPartials( const VisiblePoint& visPoint,
                       VectorR3& retPartialU, VectorR3& retPartialV ) const;

    const VectorR3& GetVertexA() const
    {
        return VertexA;
    }
    const VectorR3& GetVertexB() const
    {
        return VertexB;
    }
    const VectorR3& GetVertexC() const
    {
        return VertexC;
    }
    void GetVertices( double* verts ) const;	// Returns 9 doubles
    void GetVertices( float* verts ) const;		// Returns 9 floats
    void GetVertices( VectorR3* vertA, VectorR3* vertB, VectorR3* vertC ) const;
    const VectorR3& GetNormal() const
    {
        return Normal;
    }

protected:
    VectorR3 VertexA;
    VectorR3 VertexB;
    VectorR3 VertexC;

    VectorR3 Normal;	// Unit normal to the plane of triangle
    double PlaneCoef;	// Constant coef in def'n of the plane
    VectorR3 Ubeta;		// Vector for finding beta coef
    VectorR3 Ugamma;	// Vector for finding gamma coef

    void PreCalcInfo();				// Precalculations for intersection testing speed
};

inline ViewableTriangle::ViewableTriangle()
{
}

inline void ViewableTriangle::Init( const double* v )
{
    VertexA.Load( v );
    VertexB.Load( v+3 );
    VertexC.Load( v+6 );
    PreCalcInfo();
}

inline void ViewableTriangle::Init( const float* v )
{
    VertexA.Load( v );
    VertexB.Load( v+3 );
    VertexC.Load( v+6 );
    PreCalcInfo();
}

inline void ViewableTriangle::Init( const VectorR3& vA,
                                    const VectorR3& vB,
                                    const VectorR3& vC )
{
    VertexA = vA;
    VertexB = vB;
    VertexC = vC;
    PreCalcInfo();
}

inline void ViewableTriangle::GetVertices( double* v ) const
{
    VertexA.Dump( v );
    VertexB.Dump( v+3 );
    VertexC.Dump( v+6 );
}

inline void ViewableTriangle::GetVertices( float* v )  const
{
    VertexA.Dump( v );
    VertexB.Dump( v+3 );
    VertexC.Dump( v+6 );
}

inline void ViewableTriangle::GetVertices(
    VectorR3* vertA, VectorR3* vertB, VectorR3* vertC)  const
{
    *vertA = VertexA;
    *vertB = VertexB;
    *vertC = VertexC;
}

#endif // VIEWABLETRIANGLE_H
