/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VIEWABLEELLIPSOID_H
#define VIEWABLEELLIPSOID_H

#include "Gray/Graphics/ViewableBase.h"
#include "Gray/Graphics/Material.h"
#include "Gray/VrMath/LinearR3.h"

class ViewableEllipsoid : public ViewableBase
{

public:

    // Constructors
    ViewableEllipsoid()
    {
        Reset();
    }

    void Reset();

    // Returns an intersection if found with distance maxDistance
    // viewDir must be a unit vector.
    // intersectDistance and visPoint are returned values.
    virtual bool FindIntersectionNT (
        const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
        double *intersectDistance, VisiblePoint& returnedPoint ) const;
    void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const;
    bool CalcPartials( const VisiblePoint& visPoint,
                       VectorR3& retPartialU, VectorR3& retPartialV ) const;

    void SetCenter( double x, double y, double z );
    void SetCenter( const double *center );
    void SetCenter( const float *center );
    void SetCenter( const VectorR3& center );

    // The set central axis should be used only for ellipsoids with
    //		rotational symmetry (i.e., RadiusA = RadiusB.)
    void SetCentralAxis( const VectorR3& axisC );
    // SetAxes() will compute the third axis, axisB
    void SetAxes( const VectorR3& axisC, const VectorR3& axisA );

    void SetRadii( double radiusC, double radiusAB );
    void SetRadii( double radiusC, double radiusA, double radiusB );

    // AxisY is the center axis for calculation of u-v coordinates
    void SetuvSpherical();		// Sets u,v coords to be spherical projection
    void SetuvCylindrical();	// Sets u,v coords to be cylidrical projection
    void ResetUV();

    void SetMaterialOuter( const Material *material )
    {
        SetMaterialFront(material);
    };
    void SetMaterialInner( const Material *material )
    {
        SetMaterialBack(material);
    };

    // Get routines
    void GetCenter( double *center ) const
    {
        Center.Dump(center);
    }
    void GetCenter( float *center ) const
    {
        Center.Dump(center);
    }
    const VectorR3& GetCenter( ) const
    {
        return Center;
    }

    // These return the axes scaled by the INVERSE of the radius.  Mult by the radius to get the unit axes.
    const VectorR3& GetScaledInvCentralAxis( ) const
    {
        return AxisA;
    }
    const VectorR3& GetScaledInvAxisB( ) const
    {
        return AxisB;
    }
    const VectorR3& GetScaledInvAxisC( ) const
    {
        return AxisC;
    }
    void GetScaledInvAxes( double* axes ) const;
    void GetScaledInvAxes( float* axes ) const;

    // These return the axes as unit vectors.
    VectorR3 GetCentralAxis( ) const
    {
        return AxisA*RadiusA;
    }
    VectorR3 GetAxisB( ) const
    {
        return AxisB*RadiusB;
    }
    VectorR3 GetAxisC( ) const
    {
        return AxisC*RadiusC;
    }
    void GetAxes( double* axes ) const;
    void GetAxes( float* axes ) const;

    double GetRadiusA () const
    {
        return RadiusA;    // Radius A is contral axis radius
    }
    double GetRadiusB () const
    {
        return RadiusB;
    }
    double GetRadiusC () const
    {
        return RadiusC;
    }
    void GetRadiiScaledInv( double *radiusA, double *radiusB, double *radiusC ) const
    {
        *radiusA = RadiusA;
        *radiusB = RadiusB;
        *radiusC = RadiusC;
    }
    const Material* GetMaterialOuter()  const
    {
        return GetMaterialFront();
    };
    const Material* GetMaterialInner()  const
    {
        return GetMaterialBack();
    };
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
    int GetDetectorId() const
    {
        return -1;
    }

protected:

    VectorR3 Center;

    VectorR3 AxisA;		// Axes are orthogonal and have
    VectorR3 AxisB;		//	length equal to the inverse of the radius
    VectorR3 AxisC;		//  in that direction.
    // AxisC is the center axis.  AxisA, B, C analogous to Z, X, Y

    double RadiusA;
    double RadiusB;
    double RadiusC;

    int uvProjectionType;	// ==0 for spherical, ==1 for cylindrical
    // AxisA is the center axis (v-axis) for purposes of u-v coordinates for
    //		texture maps.

};

inline void ViewableEllipsoid::Reset()
{
    Center.SetZero();
    AxisC.SetUnitY();
    AxisA.SetUnitZ();
    AxisB.SetUnitX();
    RadiusA = 1.0;
    RadiusB = 1.0;
    RadiusC = 1.0;
    ResetUV();
}

inline void ViewableEllipsoid::SetCenter( double x, double y, double z )
{
    Center.Set(x,y,z);
}

inline void ViewableEllipsoid::SetCenter( const double* center )
{
    SetCenter( *center, *(center+1), *(center+2) );
}

inline void ViewableEllipsoid::SetCenter( const float* center )
{
    SetCenter( *center, *(center+1), *(center+2) );
}

inline void ViewableEllipsoid::SetCenter( const VectorR3& center )
{
    SetCenter( center.x, center.y, center.z );
}

// The set central axis should be used only for ellipsoids with
//		rotational symmetry (i.e., RadiusB = RadiusC.)
inline void ViewableEllipsoid::SetCentralAxis( const VectorR3& axisC )
{

    AxisC = axisC;
    assert (AxisC.Norm() != 0.0);
    AxisC.Normalize();
    GetOrtho(AxisC, AxisA, AxisB);

    AxisA /= RadiusA;
    AxisB /= RadiusB;
    AxisC /= RadiusC;
}

// SetAxes() will compute the third axis.
inline void ViewableEllipsoid::SetAxes( const VectorR3& axisC,
                                        const VectorR3& axisA )
{
    AxisA = axisA;
    AxisC = axisC;
    assert ( EqualZeroFuzzy(AxisA^AxisC) );
    AxisB = AxisC*AxisA;		// Form 3rd axis with crossproduct

    assert ( AxisA.Norm()!=0.0 && AxisB.Norm()!=0.0 && AxisC.Norm()!=0.0 );
    AxisA /= RadiusA*AxisA.Norm();
    AxisB /= RadiusB*AxisB.Norm();
    AxisC /= RadiusC*AxisC.Norm();
}

inline void ViewableEllipsoid::SetRadii( double radiusC, double radiusAB )
{
    SetRadii(radiusC, radiusAB, radiusAB);
}

inline void ViewableEllipsoid::SetRadii( double radiusC, double radiusA, double radiusB )
{
    assert(RadiusA>0 && RadiusB>0 && RadiusC>0);

    RadiusA = radiusA;
    RadiusB = radiusB;
    RadiusC = radiusC;

    AxisA /= RadiusA*AxisA.Norm();
    AxisB /= RadiusB*AxisB.Norm();
    AxisC /= RadiusC*AxisC.Norm();
}

inline void ViewableEllipsoid::GetScaledInvAxes( double* axes ) const
{
    AxisA.Dump(axes);
    AxisB.Dump(axes+3);
    AxisC.Dump(axes+6);
}

inline void ViewableEllipsoid::GetScaledInvAxes( float* axes ) const
{
    AxisA.Dump(axes);
    AxisB.Dump(axes+3);
    AxisC.Dump(axes+6);
}

inline void ViewableEllipsoid::GetAxes( double* axes ) const
{
    AxisA.Dump(axes);
    AxisB.Dump(axes+3);
    AxisC.Dump(axes+6);
    axes[0] *= RadiusA;
    axes[1] *= RadiusA;
    axes[2] *= RadiusA;
    axes[3] *= RadiusB;
    axes[4] *= RadiusB;
    axes[5] *= RadiusB;
    axes[6] *= RadiusC;
    axes[7] *= RadiusC;
    axes[8] *= RadiusC;
}

inline void ViewableEllipsoid::GetAxes( float* axes ) const
{
    AxisA.Dump(axes);
    AxisB.Dump(axes+3);
    AxisC.Dump(axes+6);
    axes[0] *= (float)RadiusA;
    axes[1] *= (float)RadiusA;
    axes[2] *= (float)RadiusA;
    axes[3] *= (float)RadiusB;
    axes[4] *= (float)RadiusB;
    axes[5] *= (float)RadiusB;
    axes[6] *= (float)RadiusC;
    axes[7] *= (float)RadiusC;
    axes[8] *= (float)RadiusC;
}

inline void ViewableEllipsoid::SetuvSpherical()
// Sets u,v coords to be spherical projection
{
    uvProjectionType = 0;
}

inline void ViewableEllipsoid::SetuvCylindrical()
// Sets u,v coords to be cylidrical projection
{
    uvProjectionType = 1;
}

inline void ViewableEllipsoid::ResetUV( )
{
    uvProjectionType = 0;		// Spherical is the default
}

#endif //VIEWABLEELLIPSE_H
