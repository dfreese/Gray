#ifndef ANNULUSCYLINDERSOURCE_H_
#define ANNULUSCYLINDERSOURCE_H_

#include <VrMath/LinearR3.h>
#include <Sources/Source.h>

class AnnulusCylinderSource : public Source
{
public:
    AnnulusCylinderSource();
    AnnulusCylinderSource(const VectorR3 &pos, double radius, VectorR3 L, double act);
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

#endif /*ANNULUSCYLINDERSOURCE_H_*/
