/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

#include <Graphics/SceneDescription.h>
#include <fstream>
#include <iostream>
#include <stack>

void SceneDescription::AddLight(std::unique_ptr<Light> newLight)
{
    LightArray.push_back(std::move(newLight));
}

void SceneDescription::AddMaterial(std::unique_ptr<Material> newMaterial) {
    int index = (int)MaterialArray.size();
    material_names_map[newMaterial->GetName()] = index;
    MaterialArray.push_back(std::move(newMaterial));
}

void SceneDescription::AddViewable(std::unique_ptr<ViewableBase> newViewable) {
    ViewableArray.push_back(std::move(newViewable));
}

// Once you have set up an initial CameraView, you can call RegisterCameraView.
//  After that, you may call CalcNewSceenDims( aspectRatio ) to get
//	a suggested width and height for the camera screen.
void SceneDescription::RegisterCameraView()
{
    RegisteredScreenWidth = CameraAndViewer.GetScreenWidth();
    RegisteredScreenHeight = CameraAndViewer.GetScreenHeight();
    ScreenRegistered = true;
}

void SceneDescription::CalcNewScreenDims( float aspectRatio )
{
    assert ( ScreenRegistered );
    assert ( aspectRatio>0.0 );
    if ( ScreenRegistered ) {
        double registeredAspectRatio = RegisteredScreenWidth/RegisteredScreenHeight;
        if ( registeredAspectRatio <= aspectRatio ) {
            // Match up heights
            CameraAndViewer.SetScreenDimensions( RegisteredScreenHeight*aspectRatio, RegisteredScreenHeight );
        } else {
            // Match up widths
            CameraAndViewer.SetScreenDimensions( RegisteredScreenWidth, RegisteredScreenWidth/aspectRatio );
        }
    }
}

AABB SceneDescription::GetExtents() const {
    AABB scene_aabb;
    for (size_t idx = 0; idx < NumViewables(); idx++) {
        AABB aabb;
        GetViewable(idx).CalcAABB(aabb);
        scene_aabb.EnlargeToEnclose(aabb);
    }
    return(scene_aabb);
}

double SceneDescription::GetMaxDistance() const {
    AABB extents = GetExtents();
    return((extents.GetBoxMax() - extents.GetBoxMin()).Norm());
}

void SceneDescription::BuildTree(bool use_double_recurse_split,
                                 double object_cost) {
    kd_tree.SetDoubleRecurseSplitting(use_double_recurse_split);
    kd_tree.SetObjectCost(object_cost);
    auto ExtentFunc = [this](long obj, AABB & box) {
        this->GetViewable(obj).CalcAABB(box);
    };
    auto ExtentInBoxFunc = [this](long obj, const AABB & enc_box, AABB & box) {
        return (this->GetViewable(obj).CalcExtentsInBox(enc_box, box));
    };
    kd_tree.BuildTree(NumViewables(), ExtentFunc, ExtentInBoxFunc);
}

bool SceneDescription::intersection_callback(
        long objectNum, const VectorR3 & start_pos, const VectorR3 & direction,
        double & retStopDistance,VisiblePoint & visible_point_return_ptr) const
{
    double thisHitDistance;
    VisiblePoint tempPoint;
    bool hitFlag = GetViewable(objectNum).FindIntersection(start_pos, direction,
                                                           retStopDistance,
                                                           &thisHitDistance,
                                                           tempPoint);
    if ( !hitFlag ) {
        return false;
    }
    visible_point_return_ptr = tempPoint;
    // No need to traverse search further than this distance in the future
    retStopDistance = thisHitDistance;
    return true;
}

long SceneDescription::SeekIntersection(const VectorR3& pos,
                                        const VectorR3& direction,
                                        double & hitDist,
                                        VisiblePoint& returnedPoint) const
{
    auto intersect_func = [this, &returnedPoint](long objectNum,
                                                 const VectorR3 & start_pos,
                                                 const VectorR3 & direction,
                                                 double & retStopDistance)
    {
        return (this->intersection_callback(objectNum, start_pos, direction,
                                            retStopDistance, returnedPoint));
    };
    return(kd_tree.Traverse(pos, direction, hitDist, intersect_func));
}

bool SceneDescription::TestOverlapSingle(VectorR3 & start, const VectorR3 & dir) const {
    std::stack<Material const *> mat_stack;
    // Start by looking as far as possible in SeekIntersection
    double hit_dist = DBL_MAX;
    VisiblePoint point;
    // negative return from Seek Intersection indicates it didn't run into
    // anything.
    while (SeekIntersection(start, dir, hit_dist, point) >= 0) {
        // Move the point to where we hit, plus an epsilon to make sure we don't
        // hit the same place again.
        start += (hit_dist + ray_trace_epsilon) * dir;
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

bool SceneDescription::TestOverlap() const {
    AABB extents = GetExtents();

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
