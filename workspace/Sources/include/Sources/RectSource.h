#ifndef GRAY_RECTSOURCE_H
#define GRAY_RECTSOURCE_H

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Sources/Source.h>


class RectSource : public Source
{
public:
    RectSource();
    RectSource(const VectorR3 &pos, const VectorR3 &sz, double act);
    void SetSize(const VectorR3 &size);

    void virtual Decay(unsigned int photon_number);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    VectorR3 size;
};

#endif /*GRAYSOURCE_H_*/
