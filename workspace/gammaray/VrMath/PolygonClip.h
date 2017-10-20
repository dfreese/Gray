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

// PolygonClip.h
//  Author: Sam Buss, October 2004.

#ifndef POLYGON_CLIP_H
#define POLYGON_CLIP_H

class VectorR3;

// Routine for clipping a convex polygon against a plane
// Returns true if clipping has occured and updates the numVerts value
// numVerts gives the number of vertices in the vertex array vertArray
// It is *always* assume that vertArray[] has enough room to
//	add a new vertex if necessary.  Space must have been preallocated before
//	the routine is called.
// The planarity assumption is not used in any essential way.
//    The important thing is that the line loop of edge vectors can
//    cross (or touch) the plane at most once.
bool ClipConvexPolygonAgainstPlane( int *numVerts, VectorR3 vertArray[],
                                    const VectorR3& PlaneNormal,
                                    double PlaneConstant );

// This version is similar, but also returns the number of new verts (0, 1, or 2)
//      except if entire polygon clipped in which case numNewVerts = -1.
bool ClipConvexPolygonAgainstPlane( int *numVerts, VectorR3 vertArray[],
                                    const VectorR3& PlaneNormal,
                                    double PlaneConstant,
                                    int *numNewVerts);

bool CalcBoundingBox(int numPoints, const VectorR3* vertArray,
                     VectorR3* extentsMin, VectorR3* extentsMax);


// Routine for clipping a convex polygon against a slab (an infinite
//		region bounded by two parallel planes.
// Returns true if clipping has occured and updates the numVerts value
// numVerts gives the number of vertices in the vertex array vertArray
// It is *always* assume that vertArray[] has enough room to
//	add a new vertex if necessary.  Space must have been preallocated before
//	the routine is called.
// The planarity assumption is not used in any essential way.
//    The important thing is that the line loop of edge vectors can
//    enter and exit the slab more than twice.
// Special handling: If the slab intersects with the polygon has area zero,
//	   then the entire polygon is clipped away.  In particular, for zero thickness
//		slab, the polygon is clipped away unless the polygon lies inside the slab plane.
//	   And polygons that are touch only the surface of the slab are entirely
//		clipped away.
bool ClipConvexPolygonAgainstSlab( int *numVerts, VectorR3 vertArray[],
                                   const VectorR3& PlaneNormal,
                                   double planeConstantMin, double planeConstantMax );


// Clip the convex polygon against a bounding box.
// Returns the new number of vertices (possibly zero)
// Only returns vertices if the polygon's intersection
//		with the box has non-zero area.
// The version that takes planeNormal uses this as the normal to
//		the plane containing the vertices --- gives more stable
//		results in near-degenerate cases.

int ClipConvexPolygonAgainstBoundingBox( int numVerts, VectorR3 vertArray[], const VectorR3& planeNormal,
        const VectorR3& boxMin, const VectorR3& boxMax );

int ClipConvexPolygonAgainstBoundingBox( int numVerts, VectorR3 vertArray[],
        const VectorR3& boxMin, const VectorR3& boxMax );

// **********************************************************
// Inlined functions
// **********************************************************

inline bool ClipConvexPolygonAgainstPlane(
    int *numVerts, VectorR3 vertArray[],
    const VectorR3& PlaneNormal, double PlaneConstant )
{
    int numNewVerts;
    return ClipConvexPolygonAgainstPlane( numVerts, vertArray, PlaneNormal, PlaneConstant,
                                          &numNewVerts );
}

#endif		// POLYGON_CLIP_H
