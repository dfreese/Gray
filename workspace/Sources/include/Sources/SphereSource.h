#ifndef CSE167_SPHERESOURCE_H
#define CSE167_SPHERESOURCE_H

#include <Sources/Source.h>

class SphereSource : public Source
{
public:
    SphereSource();
    SphereSource(const VectorR3 &pos, double radius, double act);
    void SetRadius(double r);

    virtual VectorR3 Decay(unsigned int photon_number, double time);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    double radius;
};

#endif
