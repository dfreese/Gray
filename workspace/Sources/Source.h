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

#ifndef CSE167_SOURCE_H
#define CSE167_SOURCE_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>

#include "../Graphics/Material.h"
#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"
#include "../Physics/Isotope.h"

using namespace std;

class Source {
public:
	Source();
	bool NextPhoton(Photon &p);
	double GetActivity();
	VectorR3 position;
	void SetIsotope(Isotope * i) {isotope = i; };
	void SetActivity(double act);
	void SetMaterial(MaterialBase * mat);
	void SetPosition(const VectorR3 & pos) { position = pos; }
	void SetTime(double t);
	void SetSourceNum(int i) { source_num = i; if (isotope != NULL) isotope->source_num = source_num; }
	MaterialBase * GetMaterial();
	bool isNegative();
	Isotope * GetIsotope() { return isotope; }
	const VectorR3 & GetDecayPosition() const { return isotope->GetPosition(); }
	void Reset();

	virtual bool Inside(const VectorR3 &pos) const = 0;
	virtual void Decay(unsigned int photon_number) = 0;

protected:
	Isotope * isotope;
	double Random();
	double activity;
	double time;
	MaterialBase * material;
	bool negative;
	int source_num;

};

inline bool Source::isNegative() {
	return negative;
}

inline Source::Source() {
	material = NULL;
	activity = 0.0;
	negative = false;
	isotope = NULL;
	source_num = 0;
}

inline double Source::Random() {
	return rand()/(double)RAND_MAX;
}

inline double Source::GetActivity() {
	return activity;
}

inline void Source::SetActivity(double act) {
	activity = act;
	if (act < 0.0) {
		negative = true;
        } else {
		negative = false;
        }
}

inline void Source::SetMaterial(MaterialBase * mat) {
	material = mat;
}

inline MaterialBase * Source::GetMaterial() {
	return material;
}

inline bool Source::NextPhoton(Photon & p) {
	p = isotope->NextPhoton();
}

inline void Source::SetTime(double t) {
	// FIXME: Time may be redudant in Source
	time = t;
	if (isotope != NULL) isotope->SetTime(t);
}

inline void Source::Reset() {
	//cout << "Reset\n";
	if (isotope == NULL) return;
	isotope->Reset();
}

#endif
