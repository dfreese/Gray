//
//  IntersectionKdTree.cpp
//  Gray
//
//  Created by David Freese on 2/21/17.
//
//

#include <GraphicsTrees/ShadowKdTree.h>
#include <Graphics/SceneDescription.h>
#include <Graphics/Light.h>

ShadowKdTree::ShadowKdTree(const SceneDescription & scene,
                           double object_cost,
                           bool use_double_recurse_split)
{
    ActiveScene = &scene;
    SetDoubleRecurseSplitting(use_double_recurse_split);
    SetObjectCost(use_double_recurse_split);
    BuildTree(ActiveScene->NumViewables());
}


void ShadowKdTree::ExtentFunc(long objNum, AABB& retBox)
{
    ActiveScene->GetViewable(objNum).CalcAABB( retBox );
}

bool ShadowKdTree::ExtentInBoxFunc(long objNum, const AABB& aabb,
                                      AABB& retBox)
{
    return ActiveScene->GetViewable(objNum).CalcExtentsInBox(aabb, retBox);
}


// Call back function for KdTraversal of view ray or reflection ray
bool ShadowKdTree::ObjectCallback(long objectNum, double* retStopDistance)
{
    double thisHitDistance;
    bool hitFlag;

    if (objectNum == kdTraverseAvoid) {
        hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(
                kdStartPosAvoid, kdTraverseDir, bestHitDistance,
                &thisHitDistance, tempPoint);
        if ( !hitFlag ) {
            return false;
        }
        thisHitDistance += isectEpsilon;		// Adjust back to real hit distance
    } else {
        hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(kdStartPos, kdTraverseDir,
                                                                       bestHitDistance, &thisHitDistance, tempPoint);
        if ( !hitFlag ) {
            return false;
        }
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
bool ShadowKdTree::ShadowFeeler(const VectorR3& pos,
                                const Light& light,
                                long intersectNum)
{
    kdTraverseDir = pos;
    kdTraverseDir -= light.GetPosition();
    double dist = kdTraverseDir.Norm();
    if ( dist<1.0e-7 ) {
        return true;		// Extremely close to the light!
    }
    kdTraverseDir /= dist;			// Direction from light position towards pos
    kdStartPos = light.GetPosition();
    kdTraverseFeeler = true;		// True indicates no shadowing objects
    kdTraverseAvoid = intersectNum;
    kdShadowDist = dist;
    Traverse(light.GetPosition(), kdTraverseDir, dist, true);

    return kdTraverseFeeler;	// Return whether ray is free of shadowing objects
}
