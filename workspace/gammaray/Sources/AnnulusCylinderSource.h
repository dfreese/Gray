#ifndef ANNULUSCYLINDERSOURCE_H_
#define ANNULUSCYLINDERSOURCE_H_

#include <VrMath/LinearR3.h>
#include <Sources/Source.h>

class AnnulusCylinderSource : public Source
{
public:
    AnnulusCylinderSource();
    AnnulusCylinderSource(const VectorR3 & position, double radius, VectorR3 L,
                          double activity);
    virtual VectorR3 Decay(int photon_number, double time);
    virtual bool Inside(const VectorR3 & pos) const;

private:
    double radius;
    double length;
    RigidMapR3 local_to_global;
};

#endif /*ANNULUSCYLINDERSOURCE_H_*/
