#ifndef VECTORSOURCE_H_
#define VECTORSOURCE_H_

#include <Sources/Source.h>
#include <VrMath/LinearR3.h>
#include <VrMath/Aabb.h>

class VectorSource : public Source
{
public:
    VectorSource(const double act);
    VectorSource(const double act, const VectorR3& boxMin, const VectorR3& boxMax);
    void SetMin(const VectorR3 &vert);
    void SetMax(const VectorR3 &vert);
    const VectorR3 & GetMin()
    {
        return aabb.GetBoxMin();
    }
    const VectorR3 & GetMax()
    {
        return aabb.GetBoxMax();
    }

    virtual VectorR3 Decay(unsigned int photon_number, double time);
    bool virtual Inside(const VectorR3 & pos) const;

private:
    AABB aabb;
    bool RejectionTest(const VectorR3 &pos);
};

#endif /*VECTORSOURCE_H_*/
