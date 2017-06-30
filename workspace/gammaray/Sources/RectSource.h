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
               const RigidMapR3 & map, double act);
    void SetSize(const VectorR3 &size);

    virtual VectorR3 Decay(int photon_number, double time);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    VectorR3 size;
    /*!
     * Corresponds to whatever the the matrix was when the rect source is
     * created, with a right transpose applied for the center of the source.
     * This rotates and translates the source back from it's position in space
     * to being centered at the origin.
     */
    const RigidMapR3 mapping;
    const RigidMapR3 inv_map;
};

#endif /*GRAYSOURCE_H_*/
