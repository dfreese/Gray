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

#ifndef VIEWABLEBASE_H
#define VIEWABLEBASE_H

// ****************************************************************************
// The classes    ViewableBase   and   VisiblePoint   are defined in this file.
// ****************************************************************************

#include <VrMath/LinearR3.h>
#include <VrMath/Aabb.h>
#include <Graphics/MaterialBase.h>
#include <Graphics/Material.h>
#include <Graphics/TextureMapBase.h>
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

    // Sets front and back texture maps.
    // Subclasses of ViewablePoint will have more routines.
    void TextureMap( const TextureMapBase* texture );	// Front & back
    void TextureMapFront( const TextureMapBase* texture );
    void TextureMapBack( const TextureMapBase* texture );
    void TextureMapOuter( const TextureMapBase* texture );
    void TextureMapInner( const TextureMapBase* texture );

    bool HasFrontTextureMap() const
    {
        return (TextureFront!=0);
    }
    bool HasBackTextureMap() const
    {
        return (TextureBack!=0);
    }
    bool HasInnerTextureMap() const
    {
        return (TextureFront!=0);
    }
    bool HasOuterTextureMap() const
    {
        return (TextureBack!=0);
    }

    // CalcBoundingPlanes:
    //   Computes the extents of the viewable object with respect to a
    //		a given normal direction  u.   u is the normal to a family of planes.
    //   Returns  min{u^x} and max{u^x} for x a point on the object (^ = dot product)
    //   Important: u must be a unit vector!
    virtual void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const = 0;

    // Calculate the axis aligned bounding box
    virtual void CalcAABB( AABB& retAABB ) const;

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

    unsigned GetDetectorId() const
    {
        return detector_id;
    }
    void SetDetectorId(unsigned id)
    {
        detector_id = id;
    }
    void SetSrcId(unsigned char id)
    {
        src_id = id;
    }
    unsigned char GetSrcId() const
    {
        return src_id;
    }

    void SetMaterial( const MaterialBase* material );
    void SetMaterialFront( const MaterialBase* frontmaterial );
    void SetMaterialBack( const MaterialBase* backmaterial );
    const MaterialBase* GetMaterialFront() const
    {
        return FrontMat;
    }
    const MaterialBase* GetMaterialBack() const
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
    const TextureMapBase* TextureFront;		// Front texture map
    const TextureMapBase* TextureBack;		// Back Texture map

    unsigned detector_id;
    unsigned char src_id;

    const MaterialBase* FrontMat;
    const MaterialBase* BackMat;	// Null point if not visible from back
};

inline ViewableBase::ViewableBase() :
    detector_id(-1),
    src_id(0),
    FrontMat(&Material::Default),
    BackMat(&Material::Default)
{
    TextureMap(0);
}

inline void ViewableBase::TextureMap(
    const TextureMapBase* texture )
{
    TextureMapFront( texture );
    TextureMapBack( texture );
}

inline void ViewableBase::TextureMapFront( const TextureMapBase* texture )
{
    TextureFront = texture;
}

inline void ViewableBase::TextureMapBack( const TextureMapBase* texture )
{
    TextureBack = texture;
}

inline void ViewableBase::TextureMapOuter( const TextureMapBase* texture )
{
    TextureMapFront( texture );
}

inline void ViewableBase::TextureMapInner( const TextureMapBase* texture )
{
    TextureMapBack( texture );
}

inline void ViewableBase::SetMaterial(const MaterialBase* material )
{
    SetMaterialFront(material);
    SetMaterialBack(material);
}

inline void ViewableBase::SetMaterialFront(const MaterialBase* frontmaterial )
{
    FrontMat = frontmaterial;
}

inline void ViewableBase::SetMaterialBack( const MaterialBase* backmaterial )
{
    BackMat = backmaterial;
}

inline bool ViewableBase::FindIntersection (
    const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
    double *intersectDistance, VisiblePoint& returnedPoint ) const
{
    bool found;
    found = FindIntersectionNT(viewPos, viewDir,
                               maxDistance, intersectDistance, returnedPoint);
    if ( found ) {
        returnedPoint.SetObject( this );
        // Invoke the texture map (if any)
        const TextureMapBase* texmap = returnedPoint.IsFrontFacing() ? TextureFront : TextureBack;
        if ( texmap ) {
            texmap->ApplyTexture( returnedPoint, viewDir );
        }
    }
    return found;
}

#endif // VIEWABLEBASE_H
