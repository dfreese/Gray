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

#ifndef CSE167_SPHERESOURCE_H
#define CSE167_SPHERESOURCE_H

#include "Source.h"

using namespace std;

class SphereSource : public Source
{
public:
    SphereSource();
    SphereSource(const VectorR3 &pos, double radius, double act);
    void SetRadius(double r);

    void virtual Decay(unsigned int photon_number);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    double radius;
};

#endif
