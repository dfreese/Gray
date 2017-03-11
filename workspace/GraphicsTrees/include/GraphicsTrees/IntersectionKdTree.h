//
//  IntersectionKdTree.h
//  Gray
//
//  Created by David Freese on 2/21/17.
//
//

#ifndef IntersectionKdTree_h
#define IntersectionKdTree_h

#include <KdTree/KdTree.h>
#include <Graphics/VisiblePoint.h>

class SceneDescription;
class VisiblePoint;

class IntersectKdTree : public KdTree {
public:
    IntersectKdTree(const SceneDescription & scene,
                    double object_cost=8.0,
                    bool use_double_recurse_split=true);
    virtual long SeekIntersection(const VectorR3& pos,
                                  const VectorR3& direction,
                                  double *hitDist,
                                  VisiblePoint& returnedPoint,
                                  long avoidK);

protected:
    virtual bool ObjectCallback(long objectNum, double* retStopDistance);
    virtual void ExtentFunc(long objectNum, AABB& boundingBox);
    virtual bool ExtentInBoxFunc(long objectNum, const AABB& clippingBox,
                                 AABB& boundingBox);
    SceneDescription const * ActiveScene;
    long bestObject;
    long kdTraverseAvoid;
    double bestHitDistance;
    VectorR3 kdStartPos;
    VectorR3 kdStartPosAvoid;
    VectorR3 kdTraverseDir;
    VisiblePoint* bestHitPoint;
    const double isectEpsilon = 1.0e-6;
};

#endif /* IntersectionKdTree_h */
