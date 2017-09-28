#ifndef ELLIPTICCYLINDERSOURCE_H_
#define ELLIPTICCYLINDERSOURCE_H_

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Sources/Source.h>

class EllipticCylinderSource : public Source
{
public:
    EllipticCylinderSource();
    EllipticCylinderSource(const VectorR3 &pos, double radius1, double radius2, VectorR3 &L, double act);
    virtual VectorR3 Decay(int photon_number, double time);
    bool virtual Inside(const VectorR3 & pos) const;
    void SetRadius(double r1, double r2);
    void SetAxis(VectorR3 &L);

private:
    double radius1;
    double radius2;
    double length;
    VectorR3 axis;
    RigidMapR3 local_to_global;
    RigidMapR3 global_to_local;
};

#endif /*ELLIPTICCYLINDERSOURCE_H_*/
