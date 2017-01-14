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

#ifndef DIRECTLIGHT_H
#define DIRECTLIGHT_H

#include "../VrMath/LinearR3.h"
#include "Material.h"
#include "Light.h"
#include "View.h"
#include "ViewableBase.h"

// For a view structure (may be a local viewer)
void DirectIlluminate( const VectorR3& position, const VectorR3& normal,
                       const View& view,
                       const Light& light,
                       const MaterialBase& material,
                       VectorR3& colorReturned,
                       const VectorR3& percentLit );

// For a viewpoint with explicit position
void DirectIlluminateViewPos( const VectorR3& position, const VectorR3& normal,
                              const VectorR3& ViewPos,
                              const Light& light,
                              const MaterialBase& material,
                              VectorR3& colorReturned,
                              const VectorR3& percentLit );
// For a viewpoint with explicit direction of view
//    ViewDir must a normal vector from the direction of the viewer
void DirectIlluminateViewDir( const VectorR3& position, const VectorR3& normal,
                              const VectorR3& ViewDir,
                              const Light& light,
                              const MaterialBase& material,
                              VectorR3& colorReturned,
                              const VectorR3& percentLit );

// The routines below are identical except use the VisiblePoint class


// For a view structure (may be a local viewer)
inline void DirectIlluminate( const VisiblePoint& visPoint,
                              const View& view,
                              const Light& light,
                              VectorR3& colorReturned,
                              const VectorR3& percentLit )
{
    DirectIlluminate( visPoint.GetPosition(), visPoint.GetNormal(),
                      view, light, visPoint.GetMaterial(), colorReturned,
                      percentLit  );
}

// For a viewpoint with explicit position
inline void DirectIlluminateViewPos( const VisiblePoint& visPoint,
                                     const VectorR3& ViewPos,
                                     const Light& light,
                                     VectorR3& colorReturned,
                                     const VectorR3& percentLit )
{
    DirectIlluminateViewPos( visPoint.GetPosition(), visPoint.GetNormal(),
                             ViewPos, light, visPoint.GetMaterial(), colorReturned,
                             percentLit  );
}

// For a viewpoint with explicit direction of view
//    ViewDir must a normal vector from the direction of the viewer
inline void DirectIlluminateViewDir( const VisiblePoint& visPoint,
                                     const VectorR3& ViewDir,
                                     const Light& light,
                                     VectorR3& colorReturned,
                                     const VectorR3& percentLit )
{
    DirectIlluminateViewDir( visPoint.GetPosition(), visPoint.GetNormal(),
                             ViewDir, light, visPoint.GetMaterial(), colorReturned,
                             percentLit );
}


// N = surface normal
// L = Unit vector towards light
// V = Unit vector towards viewer
// H = H vector (or null pointer)
void DirectIlluminateBasic( VectorR3& colorReturned, const MaterialBase& material,
                            const Light& light,
                            const VectorR3& percentLit, double lightAttenuation,
                            const VectorR3& N, const VectorR3& V,
                            const VectorR3& L, const VectorR3* H );

void CalcAmbientOnly( const MaterialBase& mat, const Light& light, double lightAttenuation,
                      VectorR3& colorReturned );

bool CalcLightDirAndFactor(const Light& light,
                           const VectorR3& position,
                           VectorR3 *lightVector,
                           double *lightAttenuate );


#endif // DIRECTLIGHT_H
