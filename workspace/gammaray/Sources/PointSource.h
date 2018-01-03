#ifndef POINTSOURCE_H_
#define POINTSOURCE_H_

#include <Sources/Source.h>

class PointSource : public Source
{
public:
    PointSource(const VectorR3 &p, double act);
    VectorR3 Decay() override;
    bool Inside(const VectorR3 & pos) const override;
};

#endif // POINTSOURCE_H_
