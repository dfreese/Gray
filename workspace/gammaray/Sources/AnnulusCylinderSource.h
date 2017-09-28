#ifndef ANNULUSCYLINDERSOURCE_H_
#define ANNULUSCYLINDERSOURCE_H_

#include <VrMath/LinearR3.h>
#include <Sources/Source.h>

class AnnulusCylinderSource : public Source
{
public:
    AnnulusCylinderSource();
    AnnulusCylinderSource(const VectorR3 &pos, double radius, VectorR3 L, double act);
    virtual VectorR3 Decay(int photon_number, double time);
    bool virtual Inside(const VectorR3 & pos) const;
    void SetRadius(double r);
    void SetAxis(VectorR3 L);

private:
    double radius;
    double length;
    VectorR3 axis;
    RigidMapR3 local_to_global;
    RigidMapR3 global_to_local;
};

#endif /*ANNULUSCYLINDERSOURCE_H_*/
