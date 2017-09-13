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
    long SeekIntersection(const VectorR3& pos, const VectorR3& direction,
                          double & hitDist, VisiblePoint& returnedPoint) const;
    bool TestOverlap() const;

private:
    virtual void ExtentFunc(long objectNum, AABB& boundingBox) const;
    virtual bool ExtentInBoxFunc(long objectNum, const AABB& clippingBox,
                                 AABB& boundingBox) const;

    SceneDescription const * ActiveScene;

    static bool intersection_callback(SceneDescription const * scene,
                                      VisiblePoint * visible_point_return_ptr,
                                      long objectNum, const VectorR3 & start_pos,
                                      const VectorR3 & direction,
                                      double & retStopDistance);
    bool TestOverlapSingle(VectorR3 & start, const VectorR3 & dir) const;
};

#endif /* IntersectionKdTree_h */
