#include <Sources/VectorSource.h>
#include <Graphics/ViewableTriangle.h>
#include <Random/Random.h>

using namespace std;

extern long SeekIntersectionKd(const VectorR3& startPos, const VectorR3& direction,
                               double *hitDist, VisiblePoint& returnedPoint,
                               long avoidK);

VectorSource::VectorSource(const double act)
{
    if (act < 0.0) {
        cout << "Cannot have negative vector sources\n";
        exit(0);
    }
    SetActivity(act);
}

VectorSource::VectorSource(const double act, const VectorR3& boxMin, const VectorR3& boxMax)
{
    if (act < 0.0) {
        cout << "Cannot have negative vector sources\n";
        exit(0);
    }
    SetActivity(act);
    aabb.Set(boxMin, boxMax);
}

void VectorSource::Decay(unsigned int photon_number)
{

    if (isotope == NULL) {
        return;
    }

    VectorR3 pos;

    VectorR3 delta = aabb.GetBoxMax();
    delta -= aabb.GetBoxMin();

    do {
        pos.x = aabb.GetBoxMin().x + Random::Uniform()*delta.x;
        pos.y = aabb.GetBoxMin().y + Random::Uniform()*delta.y;
        pos.z = aabb.GetBoxMin().z + Random::Uniform()*delta.z;
    } while (RejectionTest(pos));

    isotope->SetPosition(pos);
    isotope->Decay(photon_number);
    // no positron range for vector sources
    isotope->SetPosition(pos);
}

bool VectorSource::RejectionTest(const VectorR3 &pos)
{

    VectorR3 dir;
    Random::UniformSphere(dir);

    double hitDist;
    VisiblePoint visPoint;
    int avoidK = -1;

    int intersectNum = SeekIntersectionKd(pos,dir,
                                          &hitDist,visPoint,avoidK );

    if ( intersectNum<0 ) {
        return true;
    } else {

        // get the triangle that generated the hit
        const ViewableTriangle * t = (const ViewableTriangle*)&(visPoint.GetObject());

        // if the triangle is not a source find a new source

        if (t->GetSrcId() == 0) {
            return true;
        }
        if (visPoint.IsFrontFacing()) {
            return true;
        }
        if (visPoint.IsBackFacing()) {
            return false;
        }
        cout << "ERROR: material has no face\n";
        exit(1);
        return true;
    }
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
    // TODO: Fix const Inside Reference (because of rejection test)
    if (isotope == NULL) {
        return false;
    }
    return false;
    //return !(RejectionTest(isotope->GetPosition()));
}