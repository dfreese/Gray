/*
 *	InteractionList
 *
 *		Record history of interactions as they happen
 *		Output parsers will record these to output files
 *
 */

#ifndef INTERACTION_LIST_H
#define INTERACTION_LIST_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>
#include "../Graphics/MaterialBase.h"
#include "../DataStructs/Array.h"
#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"
#include "../Random/mt19937.h"
#include "Photon.h"
#include "Isotope.h"
#include "Positron.h"
#include "../Random/mt19937.h"
#include "../Output/Output.h"
#include "Deposit.h"

using namespace std;

class InteractionList {
public:
	InteractionList();
	~InteractionList();
	bool isEmpty() const { return hits.IsEmpty(); }
	Deposit & NextHit();
	void Reset();
	void HitPositron(const Positron &p, double deposit);
	void HitCompton(const Photon &p, double deposit, const MaterialBase & mat);
	void HitPhotoelectric(const Photon &p, double deposit, const MaterialBase & mat);
	void HitRayleigh(const Photon &p, const MaterialBase & mat) { };
	Array<Deposit> hits;
	friend ostream& operator<< (ostream &os, const InteractionList & l);
protected:
	void AddHit(Deposit & d);
	Isotope * cur_Isotope;
	Photon * cur_Photon;
	Deposit EMPTY;
	Deposit hit;
};

#endif
