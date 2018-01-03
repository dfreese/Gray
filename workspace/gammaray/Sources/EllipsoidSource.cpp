#include <Sources/EllipsoidSource.h>
#include <Random/Random.h>

EllipsoidSource::EllipsoidSource() :
    Source(),
    radius1(1.0),
    radius2(1.0),
    radius3(1.0),
    axis1(1.0, 0.0, 0.0),
    axis2(0.0, 1.0, 0.0),
    axis3(0.0, 0.0, 1.0)
{
    position.SetZero();
}

EllipsoidSource::EllipsoidSource(const VectorR3 &center, const VectorR3 &a1,
                                 const VectorR3 &a2, double r1, double r2,
                                 double r3, double act) :
    Source(center, act),
    radius1(r1),
    radius2(r2),
    radius3(r3)
{
    SetAxis(a1, a2);
}

VectorR3 EllipsoidSource::Decay() const {
    const double r1sq = radius1*radius1;
    const double r2sq = radius2*radius2;
    const double r3sq = radius3*radius3;
    VectorR3 p;
    // ellipsoid test
    do {
        p.x = (1.0 - 2.0*Random::Uniform())*radius1;
        p.y = (1.0 - 2.0*Random::Uniform())*radius2;
        p.z = (1.0 - 2.0*Random::Uniform())*radius3;
    } while ( (p.x*p.x/r1sq + p.y*p.y/r2sq + p.z*p.z/r3sq) > 1 );
    return(local_to_global * p);
}

void EllipsoidSource::SetRadius(double r1, double r2, double r3)
{
    radius1 = r1;
    radius2 = r2;
    radius3 = r3;
}

void EllipsoidSource::SetAxis(const VectorR3 &a1, const VectorR3 &a2)
{
    axis1 = a2;
    axis2 = a1*a2;
    axis3 = a1;
    axis = a1;
    axis.MakeUnit();
    local_to_global = RefAxisPlusTransToMap(axis, position);
    global_to_local = local_to_global.Inverse();
}

bool EllipsoidSource::Inside(const VectorR3 & pos) const
{
    VectorR3 local = global_to_local * pos;

    const double r1 = (local.x * local.x) / (radius1 * radius1);
    const double r2 = (local.y * local.y) / (radius2 * radius2);
    const double r3 = (local.z * local.z) / (radius3 * radius3);

    // ellipsoid test
    return ((r1 + r2 + r3) < 1);
}
