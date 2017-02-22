//
//  IntersectionKdTree.h
//  Gray
//
//  Created by David Freese on 2/21/17.
//
//

#ifndef ShadowKdTree_h
#define ShadowKdTree_h

#include <KdTree/KdTree.h>
#include <Graphics/VisiblePoint.h>

class SceneDescription;
class Light;

class ShadowKdTree : public KdTree {
public:
    ShadowKdTree(const SceneDescription & scene,
                 double object_cost=8.0,
                 bool use_double_recurse_split=true);
    bool ShadowFeeler(const VectorR3& pos,
                      const Light& light,
                      long intersectNum = -1);

protected:
    virtual bool ObjectCallback(long objectNum, double* retStopDistance);
    virtual void ExtentFunc(long objectNum, AABB& boundingBox);
    virtual bool ExtentInBoxFunc(long objectNum, const AABB& clippingBox,
                                 AABB& boundingBox);
    SceneDescription const * ActiveScene;
    bool kdTraverseFeeler;
    long bestObject;
    long kdTraverseAvoid;
    double bestHitDistance;
    double kdShadowDist;
    VectorR3 kdStartPos;
    VectorR3 kdStartPosAvoid;
    VectorR3 kdTraverseDir;
    VisiblePoint tempPoint;
    VisiblePoint* bestHitPoint;
    const double isectEpsilon = 1.0e-6;
};

#endif /* ShadowKdTree_h */
