#ifndef VECTORSOURCE_H_
#define VECTORSOURCE_H_

#include <Sources/Source.h>
#include <VrMath/LinearR3.h>
#include <VrMath/Aabb.h>

class IntersectKdTree;

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

    virtual VectorR3 Decay(int photon_number, double time);
    bool virtual Inside(const VectorR3 & pos) const;
    void SetKdTree(IntersectKdTree & tree) {
        kd_tree = &tree;
    }

private:
    AABB aabb;
    IntersectKdTree * kd_tree;
};

#endif /*VECTORSOURCE_H_*/
