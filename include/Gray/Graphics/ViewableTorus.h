/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VIEWABLETORUS_H
#define VIEWABLETORUS_H

#include "Gray/Graphics/Material.h"
#include "Gray/Graphics/ViewableBase.h"
#include "Gray/VrMath/LinearR4.h"

class ViewableTorus : public ViewableBase
{

public:

    ViewableTorus()
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

    void SetCenter( double x, double y, double z );
    void SetCenter( double *centerpos );
    void SetCenter( float *centerpos );
    void SetCenter( const VectorR3& centerpos );
    void SetAxis( const double* centralaxis );
    void SetAxis( const float* centralaxis );
    void SetAxis( const VectorR3& centralaxis );
    void SetRadii( double majorradius, double minorradius );

    // SetRadialAxis should be called *after* SetCenterAxis
    // The radial axes are also used for calculating u, v coordinates.
    //    the texture coordinate u=0.5 line is in the direction of the axisA.
    void SetRadialAxis( const VectorR3& axisA );

    void SetMaterialOuter( const Material *material )
    {
        ViewableBase::SetMaterialFront(material);
    };
    void SetMaterialInner( const Material *material )
    {
        ViewableBase::SetMaterialBack(material);
    };

    double GetMajorRadius() const
    {
        return MajorRadius;
    }
    double GetMinorRadius() const
    {
        return MinorRadius;
    }
    void GetCenter( double *center);
    void GetCenter( float *center);
    const VectorR3& GetCenter() const
    {
        return Center;
    }
    const VectorR3& GetAxisC() const
    {
        return AxisC;    // Central axis
    }
    const VectorR3& GetAxisA() const
    {
        return AxisA;    // Axis A (points frontward, center of texture map)
    }
    const VectorR3& GetAxisB() const
    {
        return AxisB;    // Axis B
    }
    const Material* GetMaterialOuter () const
    {
        return(ViewableBase::GetMaterialFront());
    }
    const Material* GetMaterialInner () const
    {
        return(ViewableBase::GetMaterialBack());
    }

protected:
    VectorR3 Center;		// Central Position
    VectorR3 AxisC;			// Central Axis
    double MajorRadius;
    double MinorRadius;

    VectorR3 AxisA, AxisB;		// AxisA, AxisB, AxisC form a coordinate system
    double CenterCoefA, CenterCoefB, CenterCoefC;
    double OuterRadius;

    void PreCalcInfo();		// Precalculate info for collision testing

private:
    static bool CollideTwoPlanes( double pdotn, double alpha, double dimen,
                                  bool *insideFlag, double* minDist, double *minOutside);

};

inline void ViewableTorus::Reset()
{
    MajorRadius = 2.0;
    MinorRadius = 1.0;
    Center.SetZero();
    AxisC.SetUnitY();
    AxisA.SetUnitZ();
    AxisB.SetUnitX();
    Center.SetZero();
    PreCalcInfo();
}

inline void ViewableTorus::SetRadii( double majorradius, double minorradius )
{
    MajorRadius = majorradius;
    MinorRadius = minorradius;
    PreCalcInfo();
}

inline void ViewableTorus::SetAxis( const double* centralaxis )
{
    AxisC.Load(centralaxis);
    AxisC.Normalize();
    GetOrtho( AxisC, AxisA, AxisB );		// Form right handed coordinate system
    PreCalcInfo();
}

inline void ViewableTorus::SetAxis( const float* centralaxis )
{
    AxisC.Load(centralaxis);
    AxisC.Normalize();
    GetOrtho( AxisC, AxisA, AxisB );		// Form right handed coordinate system
    PreCalcInfo();
}

inline void ViewableTorus::SetAxis( const VectorR3& centralaxis )
{
    AxisC = centralaxis;
    AxisC.Normalize();
    GetOrtho( AxisC, AxisA, AxisB );		// Form right handed coordinate system
    PreCalcInfo();
}

inline void ViewableTorus::SetRadialAxis( const VectorR3& axisA )
{
    AxisA = axisA;
    AxisA -= (AxisA^AxisC)*AxisC;		// Make perpindicular to Center Axis
    assert( AxisA.Norm()!=0.0 );		// Must not be parallel to Center Axis
    AxisA.Normalize();
    AxisB = AxisC*AxisA;				// Get third orthonormal axis (crossproduct)

    PreCalcInfo();
}

inline void ViewableTorus::SetCenter( double x, double y, double z )
{
    Center.Set(x,y,z);
    PreCalcInfo();
}

inline void ViewableTorus::SetCenter( double *centerpos )
{
    Center.Load(centerpos);
    PreCalcInfo();
}

inline void ViewableTorus::SetCenter( float *centerpos )
{
    Center.Load(centerpos);
    PreCalcInfo();
}

inline void ViewableTorus::SetCenter( const VectorR3& centerpos )
{
    Center = centerpos;
    PreCalcInfo();
}

inline void ViewableTorus::GetCenter( double* center )
{
    Center.Dump(center);
}

inline void ViewableTorus::GetCenter( float* center )
{
    Center.Dump(center);
}

#endif // VIEWABLETORUS
