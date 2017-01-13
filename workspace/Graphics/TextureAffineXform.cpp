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

#include "TextureAffineXform.h"
#include "../Graphics/VisiblePoint.h"

void TextureAffineXform::ApplyTexture( VisiblePoint& visPoint ) const
{
    double u = visPoint.GetU();
    double v = visPoint.GetV();
    visPoint.SetUV( m11*u+m12*v+m13, m21*u+m22*v+m23 );
    return;
}
