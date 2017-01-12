
// Random.h
//
//   A general purpose dynamically resizable sorted Stack
//   Implemented with templates.
//	Items are stored contiguously, for quick accessing.
//	Templated class must implement a < operator
//	Top of queue has smallest element
//
//
// Author: Peter Olcott.
// Contact: pdo@stanford.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.


#ifndef RANDOM_H
#define RANDOM_H
#include "mt19937.h"
#include "math.h"
#define FWHM_to_sigma ((double)0.644391971)

class Random
{
public:
    double Gaussian();
private:
};

double inline Random::Gaussian()
{

    // FIXME: Improve performance by reusing y2
    double x1, x2, w, y1, y2;

    do {
        x1 = 2.0 * genrand() - 1.0;
        x2 = 2.0 * genrand() - 1.0;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );

    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    // y2 = x2 * w;

    return y1;
}

#endif /* RANDOM_H */
