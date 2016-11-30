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

#ifndef POSITRON_H
#define POSITRON_H

// Function prototypes
#include "Isotope.h"
#include "PositronDecay.h"

using namespace std;

class Positron: public Isotope {
public:
	Positron();
	/*
	virtual void Decay(unsigned int photon_number) = 0;
	virtual void Reset() = 0;
	virtual ostream & print_on(ostream &) const = 0;
	virtual Photon & NextPhoton();
	*/
	int source_num;
	void SetEnergy(double e) {energy = e; };
	double GetEnergy() const { return energy; };
	void SetPositronRange(double c, double e1, double e2, double max_range);
	void SetPositronRange(double gauss, double max_range);
	PositronDecay * GetPositron() { return &p; }
	PositronDecay p;

protected:
	double energy;
	bool positronRange;
	bool positronRangeGaussian;
	bool positronRangeCusp;
	double positronFWHM; // expressed in meters
	double positronC;
	double positronK1;
	double positronK2;
	double positronMaxRange;
	void PositronRange(PositronDecay &p);
};

#endif /* POSITRON_H */
