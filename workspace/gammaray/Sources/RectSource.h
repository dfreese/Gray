#ifndef GRAY_RECTSOURCE_H
#define GRAY_RECTSOURCE_H

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Sources/Source.h>


class RectSource : public Source
{
public:
    RectSource();
    RectSource(const VectorR3 &pos, const VectorR3 &sz,
               const VectorR3 & orientation, double act);

    virtual VectorR3 Decay(int photon_number, double time);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    const VectorR3 size;
    const RigidMapR3 local_to_global;
    const RigidMapR3 global_to_local;
};

#endif /*GRAYSOURCE_H_*/
