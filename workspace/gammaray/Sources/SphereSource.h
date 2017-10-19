#ifndef CSE167_SPHERESOURCE_H
#define CSE167_SPHERESOURCE_H

#include <Sources/Source.h>

class SphereSource : public Source
{
public:
    SphereSource();
    SphereSource(const VectorR3 &pos, double radius, double act);
    VectorR3 Decay() override;
    bool Inside(const VectorR3 & pos) const override;
private:
    double radius;
};

#endif
