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

#include "Positron.h"
#include "GammaDecay.h"

using namespace std;

const double CONST_E_ZR89_GAMMA = 0.90915; /* MeV */
const double CONST_PROB_ZR89_POS = 0.227;	/* 22.7% of decays lead to a positron */

class ZR89 : public Positron
{
public:
    ZR89();
    virtual void Decay(unsigned int photon_number);
    virtual void Reset();
    virtual ostream & print_on(ostream & os) const;
protected:
    GammaDecay g;
};

#endif /* ZR89_H */
