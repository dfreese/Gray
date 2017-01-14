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

#ifndef GAMMADECAY_H
#define GAMMADECAY_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>
#include "NuclearDecay.h"
#include "Photon.h"

using namespace std;


class GammaDecay : public NuclearDecay
{
public:
    GammaDecay();
    virtual void SetPosition(const VectorR3 & p);
    virtual void Decay(unsigned int photon_number);
    virtual void Reset()
    {
        gamma.Reset();
        gamma.color = P_YELLOW;
    }
    virtual ostream & print_on( ostream& os ) const;
    virtual DECAY_TYPE GetType() const
    {
        return GAMMA;
    };
    void SetEnergy(double e)
    {
        energy = e;
    };

public:
    Photon gamma;
    double energy;
};

#endif /* GAMMADECAY_H */
