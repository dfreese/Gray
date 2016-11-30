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

#ifndef CSE167_POSITRONDECAY_H
#define CSE167_POSITRONDECAY_H

// Function prototypes
#include "NuclearDecay.h"

using namespace std;

class PositronDecay : public NuclearDecay {
public:
	PositronDecay();
	void SetPositronRange(double c, double e1, double e2, double max_range);
	void SetPositronRange(double gauss, double max_range);
	void SetAcolinearity(double theta);
	void PositronRange(VectorR3 & p);
	void ClearPositronRange() { positronRange = false; };

	virtual void SetPosition(const VectorR3 & p);
	virtual void Decay(unsigned int photon_number);
	virtual void Reset();
	virtual ostream& print_on( ostream& os ) const;
	virtual DECAY_TYPE GetType() const { return POSITRON; };

public:
	Photon blue;
	Photon red;
	
protected:
	double acolinearity;
	void Acolinearity(const VectorR3 & b, VectorR3 &r, double radians);

	// Positron Range functions
	double getExponential(const double lambda);

	// Positron Range variables
	bool positronRange;
	bool positronRangeGaussian;
	bool positronRangeCusp;
	double positronFWHM; // expressed in meters
	double positronC;
	double positronK1;
	double positronK2;
	double positronMaxRange;

};

#endif
