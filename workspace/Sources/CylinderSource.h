#ifndef CYLINDERSOURCE_H_
#define CYLINDERSOURCE_H_

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>

#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"
#include "Source.h"

using namespace std;

class CylinderSource : public Source {
public:
	CylinderSource();
	CylinderSource(const VectorR3 &pos, double radius, VectorR3 L, double act);
	void virtual Decay(unsigned int photon_number);
	bool virtual Inside(const VectorR3 & pos) const;
	void SetRadius(double r);
	void SetAxis(VectorR3 L);

private:
	double radius;
	double length;
	VectorR3 axis;
        Matrix3x3 RotMtrx;
        Matrix3x3 RotMtrxInv;
};

#endif /*CYLINDERSOURCE_H_*/
