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

#ifndef NUCLEARDECAY_H
#define NUCLEARDECAY_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>

#include "../Graphics/Material.h"
#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"
#include "../Random/mt19937.h"
#include "Photon.h"

using namespace std;

enum DECAY_TYPE {DECAY_ERROR, POSITRON, GAMMA};

class NuclearDecay {
public:
	NuclearDecay();
	void SetTime(const double t) { time = t; };
	double Random();
	friend ostream& operator<< ( ostream& os, const NuclearDecay& n );
	MaterialBase * GetMaterial();
	void SetMaterial(MaterialBase * mat);
	void SetBeam(const VectorR3 &axis, const double angle);
        int GetSourceNum(void) { return source_num;};
	virtual void SetPosition(const VectorR3 & p) = 0;
	virtual void Decay(unsigned int photon_number) = 0;
	virtual void Reset() = 0;
	virtual ostream & print_on(ostream &) const = 0;
	virtual DECAY_TYPE GetType() const = 0;


public:
	double time;
	bool beamDecay;
	double energy;
	VectorR3 pos;
	unsigned int decay_number;
	int source_num;
	
protected:
	double beam_angle;
	void BeamCone();

	VectorR3 beam_axis;	
	void RandomHemiCube(VectorR3 & p);
	void Gaussian();
	double getGaussian();
	void UniformSphere(VectorR3 & p);
    	bool gaussAccess;
    	double g1, g2;

	MaterialBase * material;

	inline friend ostream& operator<< (ostream&os,const NuclearDecay &n) {
		return n.print_on(os);
	}
};

inline MaterialBase * NuclearDecay::GetMaterial() {
	return material;
}

inline void NuclearDecay::SetMaterial(MaterialBase * mat) {
	material = mat;
}

#endif /* NUCLEARDECAY_H */
