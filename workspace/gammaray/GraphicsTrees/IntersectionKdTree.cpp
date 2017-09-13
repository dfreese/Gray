//
//  IntersectionKdTree.cpp
//  Gray
//
//  Created by David Freese on 2/21/17.
//
//

#include <GraphicsTrees/IntersectionKdTree.h>
#include <functional>
#include <fstream>
#include <stack>
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

bool IntersectKdTree::TestOverlapSingle(VectorR3 & start, const VectorR3 & dir) const {
    std::stack<MaterialBase const *> mat_stack;
    const double epsilon = 1.0e-10;
    // Start by looking as far as possible in SeekIntersection
    double hit_dist = DBL_MAX;
    VisiblePoint point;
    // negative return from Seek Intersection indicates it didn't run into
    // anything.
    while (SeekIntersection(start, dir, hit_dist, point) >= 0) {
        // Move the point to where we hit, plus an epsilon to make sure we don't
        // hit the same place again.
        start += (hit_dist + epsilon) * dir;
        if (point.IsFrontFacing()) {
            // Front face means we are entering a material.
            mat_stack.push(&point.GetMaterial());
        } else if (point.IsBackFacing()) {
            // Back face means we are exiting a material
            if (mat_stack.empty()) {
                // If we somehow have an empty stack, then we somehow missed a
                // front face.
                return (false);
            }
            if (mat_stack.top() != (&point.GetMaterial())) {
                // If the material we find on the back face isn't the material
                // we think we're in, then there's probably some weird overlap.
                return (false);
            }
            // If everything looks okay, pull that material off of the stack.
            mat_stack.pop();
        }
        // Reset the hit distance, so SeekIntersection looks as far as possible.
        hit_dist = DBL_MAX;
    }
    return (true);
}

bool IntersectKdTree::TestOverlap() const {
    AABB extents = ActiveScene->GetExtents();

    const VectorR3 x_dir(1, 0, 0);
    const VectorR3 y_dir(0, 1, 0);
    const VectorR3 z_dir(0, 0, 1);

    // TODO: allow these to be set by the user, so they can be more or less
    // aggressive in testing.
    const int no_steps_per_dir = 400;
    const double failure_threshold = 0.001;

    const double x_width = extents.GetMaxX() - extents.GetMinX();
    const double y_width = extents.GetMaxY() - extents.GetMinY();
    const double z_width = extents.GetMaxZ() - extents.GetMinZ();

    const double x_step = x_width / no_steps_per_dir;
    const double y_step = y_width / no_steps_per_dir;
    const double z_step = z_width / no_steps_per_dir;

    long no_failures = 0;


    // Start back from the face of the AABB by half a step, so we don't
    // accidently start on a face of an object that is partially axis-aligned.
    const double x_start = extents.GetMinX() - x_step;
    const double y_start = extents.GetMinY() - y_step;
    const double z_start = extents.GetMinZ() - z_step;

    // Then step over a grid of points on each negative face of the AABB and
    // trace across the scene normal to that face.

    std::string report_name = "overlap_test_failures.dat";
    std::ofstream report(report_name);

    // Face: XY, Dir: Z
    for (double x = extents.GetMinX() + x_step / 2; x < extents.GetMaxX(); x += x_step) {
        for (double y = extents.GetMinY() + y_step / 2; y < extents.GetMaxY(); y += y_step) {
            VectorR3 start(x, y, z_start);
            if (!TestOverlapSingle(start, z_dir)) {
                ++no_failures;
                report << start.x << " " << start.y << " " << start.z << " " << "\n";
            }
        }
    }

    // Face: XZ, Dir: Y
    for (double x = extents.GetMinX() + x_step / 2; x < extents.GetMaxX(); x += x_step) {
        for (double z = extents.GetMinZ() + z_step / 2; z < extents.GetMaxZ(); z += z_step) {
            VectorR3 start(x, y_start, z);
            if (!TestOverlapSingle(start, y_dir)) {
                ++no_failures;
                report << start.x << " " << start.y << " " << start.z << " " << "\n";
            }
        }
    }

    // Face: YZ, Dir: X
    for (double y = extents.GetMinY() + y_step / 2; y < extents.GetMaxY(); y += y_step) {
        for (double z = extents.GetMinZ() + z_step / 2; z < extents.GetMaxZ(); z += z_step) {
            VectorR3 start(x_start, y, z);
            if (!TestOverlapSingle(start, x_dir)) {
                ++no_failures;
                report << start.x << " " << start.y << " " << start.z << " " << "\n";
            }
        }
    }
    report.close();

    float failure_rate = (float) no_failures / (float) (3 * no_steps_per_dir * no_steps_per_dir);
    cout << "Failure threshold: " << failure_threshold << endl;
    cout << "Failure rate     : " << failure_rate << endl;
    cout << "Failing points recorded in \"" << report_name << "\"" << endl;
    return (failure_rate > failure_threshold);
}
