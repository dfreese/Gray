#ifndef CYLINDERSOURCE_H_
#define CYLINDERSOURCE_H_

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Sources/Source.h>

class CylinderSource : public Source
{
public:
    CylinderSource();
    CylinderSource(const VectorR3 &pos, double radius, VectorR3 L, double act);
    virtual VectorR3 Decay(int photon_number, double time);
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
