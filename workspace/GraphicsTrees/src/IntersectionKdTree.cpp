//
//  IntersectionKdTree.cpp
//  Gray
//
//  Created by David Freese on 2/21/17.
//
//

#include <GraphicsTrees/IntersectionKdTree.h>
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


void IntersectKdTree::ExtentFunc(long objNum, AABB& retBox)
{
    ActiveScene->GetViewable(objNum).CalcAABB( retBox );
}

bool IntersectKdTree::ExtentInBoxFunc(long objNum, const AABB& aabb,
                                      AABB& retBox)
{
    return ActiveScene->GetViewable(objNum).CalcExtentsInBox(aabb, retBox);
}


// Call back function for KdTraversal of view ray or reflection ray
bool IntersectKdTree::ObjectCallback(long objectNum, double* retStopDistance)
{
    double thisHitDistance;
    bool hitFlag;
    VisiblePoint tempPoint;
    hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(
            kdStartPos, kdTraverseDir, bestHitDistance, &thisHitDistance,
            tempPoint);
    if ( !hitFlag ) {
        return false;
    }
    *bestHitPoint = tempPoint;		// The visible point that was hit
    bestObject = objectNum;				// The object that was hit
    bestHitDistance = thisHitDistance;
    *retStopDistance = bestHitDistance;	// No need to traverse search further than this distance
    return true;
}



// SeekIntersectionKd seeks for an intersection with all viewable objects
// If it finds one, it returns the index of the viewable object,
//   and sets the value of hitDist and fills in the returnedPoint values.
// This "Kd" version uses the Kd-Tree
long IntersectKdTree::SeekIntersection(const VectorR3& pos,
                                       const VectorR3& direction,
                                       double & hitDist,
                                       VisiblePoint& returnedPoint)
{
    bestObject = -1;
    bestHitDistance = DBL_MAX;
    kdStartPos = pos;
    kdTraverseDir = direction;
    bestHitPoint = &returnedPoint;

    Traverse(pos, direction);

    if ( bestObject>=0 ) {
        // FIXME: NAN in KDTREE Traversal
        if(std::isnan(bestHitDistance)) {
            hitDist = DBL_MAX;
            bestObject = -1;
        } else {
            hitDist = bestHitDistance;
        }
    }
    return bestObject;
}
