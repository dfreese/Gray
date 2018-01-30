/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef VIEWABLEBASE_H
#define VIEWABLEBASE_H

// ****************************************************************************
// The classes    ViewableBase   and   VisiblePoint   are defined in this file.
// ****************************************************************************

#include <VrMath/LinearR3.h>
#include <VrMath/Aabb.h>
#include <Graphics/Material.h>
#include <Graphics/VisiblePoint.h>

// This is the purely abstract base class for viewable objects.
//		Any ViewableBase class is responsible for determining
//		if it intersects a given line of sight.  Thus it must
//		perform intersection and distance tests, and when successful
//		must return depth information.
class ViewableBase
{

public:
    ViewableBase();
    virtual ~ViewableBase() {};

    // Returns an intersection if found with distance maxDistance
    // viewDir must be a unit vector.
    // intersectDistance and visPoint are returned values.
    bool FindIntersection (
        const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
        double *intersectDistance, VisiblePoint& returnedPoint ) const;

    // CalcBoundingPlanes:
    //   Computes the extents of the viewable object with respect to a
    //		a given normal direction  u.   u is the normal to a family of planes.
    //   Returns  min{u^x} and max{u^x} for x a point on the object (^ = dot product)
    //   Important: u must be a unit vector!
    virtual void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const = 0;

    // Calculate the axis aligned bounding box
    virtual void CalcAABB( AABB& retAABB ) const;

    // Return the axis aligned bounding box
    AABB GetAABB() const {
        AABB ret;
        CalcAABB(ret);
        return (ret);
    }

    // Calculate the extent intersected with a bounding box
    virtual bool CalcExtentsInBox( const AABB& aabb, AABB& retAABB ) const;

    // CalcPartials:
    //   Returns partial derivatives with respect to u and v.
    //   Letting p(u,v) be the point on the surface with coord's u,v;
    //   CalcPartials returns (partial p)/(partial u) and (partial p)/(partial v).
    //		in the vectors retPartialU and retPartialV.
    //	 If at a singularity (so either partial is zero or infinity, then the boolean
    //		return value is FALSE.  Otherwise the boolean return value is TRUE to indicate
    //		that the returned partial derivative information is valid.
    // Needed only for bump mapping at present.
    virtual bool CalcPartials( const VisiblePoint& visPoint,
                               VectorR3& retPartialU, VectorR3& retPartialV ) const = 0;

    int GetDetectorId() const
    {
        return detector_id;
    }
    void SetDetectorId(int id)
    {
        detector_id = id;
    }
    void SetSrcId(int id)
    {
        src_id = id;
    }
    int GetSrcId() const
    {
        return src_id;
    }

    void SetMaterial( const Material* material );
    void SetMaterialFront( const Material* frontmaterial );
    void SetMaterialBack( const Material* backmaterial );
    const Material* GetMaterialFront() const
    {
        return FrontMat;
    }
    const Material* GetMaterialBack() const
    {
        return BackMat;
    }

protected:
    // The "NT" version is the one that does all the work of finding
    //		the intersection point, and computing u,v coordinates.
    //	The "NT" version does not call the texture map: this is left for
    //		the non-NT version to do.
    virtual bool FindIntersectionNT (
        const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
        double *intersectDistance, VisiblePoint& returnedPoint ) const = 0;

private:
    int detector_id;
    int src_id;

    const Material* FrontMat;
    const Material* BackMat;	// Null point if not visible from back
};

inline ViewableBase::ViewableBase() :
    detector_id(-1),
    src_id(0),
    FrontMat(&Material::Default),
    BackMat(&Material::Default)
{
}

inline void ViewableBase::SetMaterial(const Material* material )
{
    SetMaterialFront(material);
    SetMaterialBack(material);
}

inline void ViewableBase::SetMaterialFront(const Material* frontmaterial )
{
    FrontMat = frontmaterial;
}

inline void ViewableBase::SetMaterialBack( const Material* backmaterial )
{
    BackMat = backmaterial;
}

inline bool ViewableBase::FindIntersection (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    bool found = FindIntersectionNT(viewPos, viewDir, maxDistance,
                                    intersectDistance, returnedPoint);
    if (found) {
        returnedPoint.SetObject(this);
    }
    return found;
}

#endif // VIEWABLEBASE_H
