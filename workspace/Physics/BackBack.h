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

#ifndef BackBack_H
#define BackBack_H

#include "Positron.h"

using namespace std;

class BackBack : public Positron
{
public:
    BackBack();
    virtual void Decay(unsigned int photon_number);
    virtual void Reset();
    virtual ostream & print_on(ostream & os) const;
};

#endif /* BackBack_H */
