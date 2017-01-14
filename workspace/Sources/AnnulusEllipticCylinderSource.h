#ifndef ANNULUSELLIPTICCYLINDERSOURCE_H_
#define ANNULUSELLIPTICCYLINDERSOURCE_H_

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <algorithm>
#include <vector>

#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"
#include "Source.h"

using namespace std;

#define NUM_TABLE 100000

class AnnulusEllipticCylinderSource : public Source
{
public:
    ~AnnulusEllipticCylinderSource();
    AnnulusEllipticCylinderSource();
    AnnulusEllipticCylinderSource(const VectorR3 &pos, double radius1, double radius2, VectorR3 &L, double act);
    void virtual Decay(unsigned int photon_number);
    bool virtual Inside(const VectorR3 & pos) const;
    void SetRadius(double r1, double r2);
    void SetAxis(VectorR3 &L);
    double EllipticE(double m);
    double InverseEllipticE(double arc_length);
    double EllipticK(double m);
    double IncompleteEllipticE(double phi, double m);

private:
    double radius1;
    double radius2;
    double length;
    vector<double>circ;
    VectorR3 axis;
    Matrix3x3 RotMtrx;
    Matrix3x3 RotMtrxInv;
};

#endif /*ANNULUSELLIPTICCYLINDERSOURCE_H_*/
