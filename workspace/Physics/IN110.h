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

#ifndef IN110_H
#define IN110_H

#include "Positron.h"
#include "GammaDecay.h"

using namespace std;

const double CONST_E_IN110m_GAMMA = 0.657750; /* MeV */
const double CONST_PROB_IN110m_POS = 0.61;	/* 61% of decays lead to a positron */

class IN110 : public Positron {
public:
	IN110();
	virtual void Decay(unsigned int photon_number);
	virtual void Reset();
	virtual ostream & print_on(ostream & os) const;
	// TODO: Model time between Gamma and Positron Decay accurately
	GammaDecay g;
};

#endif /* IN110_H */
