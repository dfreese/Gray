/*
 * SimpleNurbs.h
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 */

#ifndef ZR89_H
#define ZR89_H

#include <Physics/GammaPositron.h>

class ZR89 : public GammaPositron
{
public:
    ZR89(double acolinearity_deg_fwhm);
protected:
    const double zr89_max_beta_energy_mev = 0.315;

};

#endif /* ZR89_H */
