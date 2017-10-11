#include <Sources/CylinderSource.h>
#include <Random/Random.h>

CylinderSource::CylinderSource() :
    Source(),
    radius(1.0),
    length(1.0),
    axis(0.0, 0.0, 0.0)
{
}

CylinderSource::CylinderSource(const VectorR3 &p, double rad, VectorR3 L, double act) :
    Source(p, act)
{
    SetRadius(rad);
    SetAxis(L.MakeUnit());
}

VectorR3 CylinderSource::Decay(int photon_number, double time)
{
    VectorR3 positron = local_to_global * Random::UniformCylinder(length, radius);
    isotope->Decay(photon_number, time, source_num, positron);
    return(positron);
}

void CylinderSource::SetRadius(double r)
{
    radius = r;
}

void CylinderSource::SetAxis(VectorR3 L)
{
    length = L.Norm();
    axis = L.MakeUnit();
    local_to_global = RefAxisPlusTransToMap(axis, position);
    global_to_local = local_to_global.Inverse();
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
