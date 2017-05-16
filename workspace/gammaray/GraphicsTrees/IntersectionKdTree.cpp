//
//  IntersectionKdTree.cpp
//  Gray
//
//  Created by David Freese on 2/21/17.
//
//

#include <GraphicsTrees/IntersectionKdTree.h>
#include <functional>
#include <Graphics/SceneDescription.h>

IntersectKdTree::IntersectKdTree(const SceneDescription & scene,
                                 double object_cost,
                                 bool use_double_recurse_split)
{
    ActiveScene = &scene;
    SetDoubleRecurseSplitting(use_double_recurse_split);
    SetObjectCost(object_cost);
    BuildTree(ActiveScene->NumViewables());
}


void IntersectKdTree::ExtentFunc(long objNum, AABB& retBox) const
{
    ActiveScene->GetViewable(objNum).CalcAABB( retBox );
}

bool IntersectKdTree::ExtentInBoxFunc(long objNum, const AABB& aabb,
                                      AABB& retBox) const
{
    return ActiveScene->GetViewable(objNum).CalcExtentsInBox(aabb, retBox);
}

bool IntersectKdTree::intersection_callback(SceneDescription const * scene,
                                            VisiblePoint * visible_point_return_ptr,
                                            long objectNum,
                                            const VectorR3 & start_pos,
                                            const VectorR3 & direction,
                                            double & retStopDistance)
{
    double thisHitDistance;
    VisiblePoint tempPoint;
    bool hitFlag = scene->GetViewable(objectNum).FindIntersection(
            start_pos, direction, retStopDistance, &thisHitDistance, tempPoint);
    if ( !hitFlag ) {
        return false;
    }
    *visible_point_return_ptr = tempPoint;
    // No need to traverse search further than this distance in the future
    retStopDistance = thisHitDistance;
    return true;
}

long IntersectKdTree::SeekIntersection(const VectorR3& pos,
                                       const VectorR3& direction,
                                       double & hitDist,
                                       VisiblePoint& returnedPoint) const
{
    auto intersect_func = bind(&IntersectKdTree::intersection_callback,
                               ActiveScene, &returnedPoint, placeholders::_1,
                               placeholders::_2, placeholders::_3,
                               placeholders::_4);
    return(Traverse(pos, direction, hitDist, intersect_func));
}
