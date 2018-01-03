#include <Sources/CylinderSource.h>
#include <Random/Random.h>

CylinderSource::CylinderSource() :
    CylinderSource({0, 0, 0}, 1.0, {0, 0, 1}, 0)
{
}

CylinderSource::CylinderSource(const VectorR3 &p, double radius, VectorR3 L, double act) :
    Source(p, act),
    radius(radius),
    length(L.Norm()),
    axis(L.MakeUnit()),
    local_to_global(RefAxisPlusTransToMap(axis, position)),
    global_to_local(local_to_global.Inverse())
{
}

VectorR3 CylinderSource::Decay() const {
    return(local_to_global * Random::UniformCylinder(length, radius));
}

bool CylinderSource::Inside(const VectorR3 & pos) const
{
    // TODO: refactor this out for all cylinders.
    const VectorR3 roted = global_to_local * pos;
    if ((roted.x * roted.x + roted.y * roted.y) > radius * radius) {
        return false;
    }
    if (std::abs(roted.z) > length/2.0) {
        return false;
    }
    return true;
}
