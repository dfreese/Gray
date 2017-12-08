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
    VectorR3 Decay() override;
    bool Inside(const VectorR3 & pos) const override;

private:
    const double radius;
    const double length;
    const VectorR3 axis;
    const RigidMapR3 local_to_global;
    const RigidMapR3 global_to_local;
};

#endif /*CYLINDERSOURCE_H_*/
