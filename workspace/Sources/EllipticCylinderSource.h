#ifndef ELLIPTICCYLINDERSOURCE_H_
#define ELLIPTICCYLINDERSOURCE_H_

#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"
#include "Source.h"

class EllipticCylinderSource : public Source
{
public:
    EllipticCylinderSource();
    EllipticCylinderSource(const VectorR3 &pos, double radius1, double radius2, VectorR3 &L, double act);
    void virtual Decay(unsigned int photon_number);
    bool virtual Inside(const VectorR3 & pos) const;
    void SetRadius(double r1, double r2);
    void SetAxis(VectorR3 &L);

private:
    double radius1;
    double radius2;
    double length;
    VectorR3 axis;
    Matrix3x3 RotMtrx;
    Matrix3x3 RotMtrxInv;
};

#endif /*ELLIPTICCYLINDERSOURCE_H_*/
