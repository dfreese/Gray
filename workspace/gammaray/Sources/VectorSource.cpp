#include <Sources/VectorSource.h>
#include <Graphics/ViewableTriangle.h>
#include <Random/Random.h>
#include <GraphicsTrees/IntersectionKdTree.h>

using namespace std;

VectorSource::VectorSource(const double act) :
    Source({0, 0, 0}, act),
    kd_tree(NULL)
{
    if (act < 0.0) {
        cout << "Cannot have negative vector sources\n";
        exit(0);
    }
}

VectorSource::VectorSource(const double act, const VectorR3& boxMin,
                           const VectorR3& boxMax) :
    Source({0, 0, 0}, act),
    aabb(boxMin, boxMax),
    kd_tree(NULL)
{
    if (act < 0.0) {
        cout << "Cannot have negative vector sources\n";
        exit(0);
    }
}

VectorR3 VectorSource::Decay(int photon_number, double time)
{
    VectorR3 pos;

    VectorR3 delta = aabb.GetBoxMax();
    delta -= aabb.GetBoxMin();

    do {
        pos.x = aabb.GetBoxMin().x + Random::Uniform()*delta.x;
        pos.y = aabb.GetBoxMin().y + Random::Uniform()*delta.y;
        pos.z = aabb.GetBoxMin().z + Random::Uniform()*delta.z;
    } while (!Inside(pos));

    isotope->Decay(photon_number, time, source_num, pos);
    return(pos);
}

void VectorSource::SetMin(const VectorR3 &vert)
{
    if (aabb.GetMinX() > vert.x) {
        aabb.GetBoxMin().x = vert.x;
    }
    if (aabb.GetMinY() > vert.y) {
        aabb.GetBoxMin().y = vert.y;
    }
    if (aabb.GetMinZ() > vert.z) {
        aabb.GetBoxMin().z = vert.z;
    }
}

void VectorSource::SetMax(const VectorR3 &vert)
{
    if (aabb.GetMaxX() < vert.x) {
        aabb.GetBoxMax().x = vert.x;
    }
    if (aabb.GetMaxY() < vert.y) {
        aabb.GetBoxMax().y = vert.y;
    }
    if (aabb.GetMaxZ() < vert.z) {
        aabb.GetBoxMax().z = vert.z;
    }
}

bool VectorSource::Inside(const VectorR3 & pos) const
{
    VectorR3 dir;
    Random::UniformSphere(dir);

    double hitDist;
    VisiblePoint visPoint;
    long intersectNum = kd_tree->SeekIntersection(pos, dir, hitDist, visPoint);

    if (intersectNum < 0) {
        return false;
    }

    // TODO: define a kd-tree per sources to determine if it's inside.
    // get the triangle that generated the hit
    const ViewableTriangle & tri = dynamic_cast<const ViewableTriangle&>(visPoint.GetObject());
    // if the triangle is not a source find a new source
    if (tri.GetSrcId() == 0) {
        return false;
    }
    if (visPoint.IsFrontFacing()) {
        return false;
    }
    if (visPoint.IsBackFacing()) {
        return true;
    }
    throw(runtime_error("Material has no face"));
}
