/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VIEWABLESPHERE_H
#define VIEWABLESPHERE_H

#include "Gray/Graphics/ViewableBase.h"
#include "Gray/Graphics/Material.h"

class ViewableSphere : public ViewableBase
{

public:

    // Constructors
    ViewableSphere();
    ViewableSphere( const VectorR3& center, double radius,
                    const Material *material=&Material::Default );

    // Returns an intersection if found with distance maxDistance
    // viewDir must be a unit vector.
    // intersectDistance and visPoint are returned values.
    virtual bool FindIntersectionNT (
        const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
        double *intersectDistance, VisiblePoint& returnedPoint ) const;
    void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const;
    bool CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const;

    // QuickIntersectTest returns (a) if hit occurs, and (b) distance.
    //		Useful for things like bounding sphere tests
    static bool QuickIntersectTest(
        const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
        double* intersectDistance,
        const VectorR3& centerPos, double radiusSq );

    bool QuickIntersectTest(
        const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
        double* intersectDistance) const;


    void SetRadius( double radius );
    void SetMaterialOuter( const Material *material )
    {
        ViewableBase::SetMaterialFront(material);
    };
    void SetMaterialInner( const Material *material )
    {
        ViewableBase::SetMaterialBack(material);
    };
    void SetCenter( double x, double y, double z );
    void SetCenter( const double *center );
    void SetCenter( const float *center );
    void SetCenter( const VectorR3& center )
    {
        Center = center;
    }

    void SetuvAxes( const double* axisA, const double* axisC);
    void SetuvAxes( const float* axisA, const float* axisC);
    void SetuvAxes( const VectorR3& axisA, const VectorR3& axisC);
    void SetuvSpherical();		// Sets u,v coords to be spherical projection
    void SetuvCylindrical();	// Sets u,v coords to be cylidrical projection
    void ResetUV();

    double GetRadius () const
    {
        return Radius;
    }
    double GetRadiusSq () const
    {
        return RadiusSq;
    }
    const Material* GetMaterialOuter () const
    {
        return(ViewableBase::GetMaterialFront());
    }
    const Material* GetMaterialInner () const
    {
        return(ViewableBase::GetMaterialBack());
    }
    void GetCenter( double *center);
    void GetCenter( float *center);
    const VectorR3& GetCenter() const
    {
        return Center;
    }
    int GetUVType() const
    {
        return uvProjectionType;
    }
    bool IsUVSpherical() const
    {
        return (uvProjectionType==0);
    }
    bool IsUVCylindrical() const
    {
        return (uvProjectionType==1);
    }
    const VectorR3& GetAxisA() const
    {
        return AxisA;
    }
    const VectorR3& GetAxisB() const
    {
        return AxisB;
    }
    const VectorR3& GetAxisC() const
    {
        return AxisC;
    }

protected:

    double Radius;
    double RadiusSq;
    VectorR3 Center;

    int uvProjectionType;	// ==0 for spherical, ==1 for cylindrical
    VectorR3 AxisA;			// Axis for u = 0.5	(like z-axis)
    VectorR3 AxisB;			// Axis for u = 3/4. (like x-axis)
    VectorR3 AxisC;			// Axis for v.		(like y-axis)

};

inline
ViewableSphere::ViewableSphere( )
{
    SetRadius ( 1.0 );
    SetCenter ( VectorR3::Zero );
    SetMaterialOuter( &Material::Default );
    SetMaterialInner( &Material::Default );
    ResetUV();
}


inline
ViewableSphere::ViewableSphere( const VectorR3& center, double radius,
                                const Material *material )
{
    SetRadius ( radius );
    SetCenter ( center );
    if ( material ) {
        SetMaterial( material );
    } else {
        SetMaterial( &Material::Default );
    }
    ResetUV();
}

inline
void ViewableSphere::SetuvSpherical()		// Sets u,v coords to be spherical projection
{
    uvProjectionType = 0;
}

inline
void ViewableSphere::SetuvCylindrical()		// Sets u,v coords to be cylidrical projection
{
    uvProjectionType = 1;
}

inline
void ViewableSphere::ResetUV( )
{
    VectorR3 aAxis(0.0, 0.0, 1.0);
    VectorR3 cAxis(0.0, 1.0, 0.0);
    SetuvAxes( aAxis, cAxis );
    uvProjectionType = 0;		// Spherical is the default
}

inline
void ViewableSphere::SetuvAxes(	const VectorR3& axisA, const VectorR3& axisC)
{
    AxisA = axisA;
    AxisC = axisC;
    AxisA.Normalize();
    AxisC.Normalize();
    AxisB = AxisC*AxisA;
    AxisB.Normalize();			// Really ought to be normalized already
}

inline
void ViewableSphere::SetuvAxes( const double* axisA, const double* axisC)
{
    VectorR3 uza;
    uza.Load(axisA);
    VectorR3 va;
    va.Load(axisC);
    SetuvAxes(uza, va);
}

inline
void ViewableSphere::SetuvAxes( const float* axisA, const float* axisC)
{
    VectorR3 uza;
    uza.Load(axisA);
    VectorR3 va;
    va.Load(axisC);
    SetuvAxes(uza, va);
}


inline void ViewableSphere::SetRadius (double radius)
{
    Radius = radius;
    RadiusSq = radius*radius;
}

inline void ViewableSphere::SetCenter( double x, double y, double z )
{
    Center.Set(x, y, z);
}

inline void ViewableSphere::SetCenter( const double *center )
{
    SetCenter( *center, *(center+1), *center+2 );
}

inline void ViewableSphere::SetCenter( const float *center )
{
    SetCenter( *center, *(center+1), *center+2 );
}

inline void ViewableSphere::GetCenter( double* center )
{
    Center.Dump(center);
}

inline void ViewableSphere::GetCenter( float* center )
{
    Center.Dump(center);
}

inline bool ViewableSphere::QuickIntersectTest(
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double* intersectDistance ) const
{
    return QuickIntersectTest( viewPos, viewDir, maxDistance,
                               intersectDistance, Center, RadiusSq );
}


#endif // VIEWABLESPHERE_H
