/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VIEWABLEPARALLELOGRAM_H
#define VIEWABLEPARALLELOGRAM_H

#include "Gray/Graphics/Material.h"
#include "Gray/Graphics/ViewableBase.h"
#include <assert.h>

// This class subsumes reactangles and squares

class ViewableParallelogram : public ViewableBase
{

public:
    ViewableParallelogram ();

    // The Init() routines specify 3 (THREE) vertices in counterclockwise order
    //		with the 4th vertex computed from the rest.
    void Init( const double* vertexpositions);
    void Init( const float* vertexpositions);
    void Init( const VectorR3& vA, const VectorR3& vB, const VectorR3& vC );

    bool IsTwoSided() const
    {
        return (BackMat!=0);
    }
    bool BackFaceCulled() const
    {
        return (BackMat==0);
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
    const VectorR3& GetVertexD() const
    {
        return VertexD;
    }
    void GetVertices( double* vert ) const;
    void GetVertices( float* vert ) const;
    void GetVertices( VectorR3* vA, VectorR3* vB, VectorR3* vC, VectorR3 *vD ) const;
    const VectorR3& GetVertex( int vertnum ) const;
    const VectorR3& GetNormal() const
    {
        return Normal;
    }

protected:
    VectorR3 VertexA;
    VectorR3 VertexB;
    VectorR3 VertexC;
    VectorR3 VertexD;

    const Material* FrontMat;
    const Material* BackMat;	// Null point if not visible from back

    VectorR3 Normal;			// Unit normal to the plane of Parallelogram
    double PlaneCoef;			// Constant coef in def'n of the plane
    VectorR3 NormalAB, NormalBC;			// Unit vectors in from edges AB and BC
    double CoefAB, CoefBC, CoefCD, CoefDA;	// Coefs for line equations of edges
    double LengthAB, LengthBC;		// Edge lengths

    void PreCalcInfo();				// Precalculations for intersection testing speed
};

inline ViewableParallelogram::ViewableParallelogram ()
{
    SetMaterial(&Material::Default);
    VertexA.SetZero();
    VertexB.SetUnitX();
    VertexC.Set(1.0,1.0,0.0);
    PreCalcInfo();
}

// The Init() routines specify 3 (THREE) vertices in counterclockwise order
//		with the 4th vertex computed from the rest.
inline void ViewableParallelogram::Init( const double* vertexpositions)
{
    VertexA.Load( vertexpositions );
    VertexB.Load( vertexpositions+3 );
    VertexC.Load( vertexpositions+6 );
    PreCalcInfo();
}

inline void ViewableParallelogram::Init( const float* vertexpositions)
{
    VertexA.Load( vertexpositions );
    VertexB.Load( vertexpositions+3 );
    VertexC.Load( vertexpositions+6 );
    PreCalcInfo();
}

inline void ViewableParallelogram::Init(const VectorR3& vA, const VectorR3& vB, const VectorR3& vC )
{
    VertexA = vA;
    VertexB = vB;
    VertexC = vC;
    PreCalcInfo();
}

inline void ViewableParallelogram::GetVertices( double* verts ) const
{
    VertexA.Dump( verts );
    VertexB.Dump( verts+3 );
    VertexC.Dump( verts+6 );
    VertexD.Dump( verts+9 );
}

inline void ViewableParallelogram::GetVertices( float* verts ) const
{
    VertexA.Dump( verts );
    VertexB.Dump( verts+3 );
    VertexC.Dump( verts+6 );
    VertexD.Dump( verts+9 );
}

inline void ViewableParallelogram::GetVertices( VectorR3* vA, VectorR3* vB, VectorR3* vC, VectorR3* vD ) const
{
    *vA = VertexA;
    *vB = VertexB;
    *vC = VertexC;
    *vD = VertexD;
}

inline const VectorR3& ViewableParallelogram::GetVertex( int vertnum ) const
{
    assert (0 <= vertnum && vertnum <= 3);
    switch( vertnum ) {
    case 0:
        return VertexA;
    case 1:
        return VertexB;
    case 2:
        return VertexC;
    case 3:
        return VertexD;
    }
    assert(0);
    return VertexA;		// Never happens
}

#endif // VIEWABLEPARALLELOGRAM_H
