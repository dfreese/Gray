#include <Sources/EllipticCylinderSource.h>
#include <Random/Random.h>

EllipticCylinderSource::EllipticCylinderSource() :
    Source(),
    radius1(1.0),
    radius2(1.0),
    length(1.0),
    axis(0.0, 0.0, 1.0)
{
    position.SetZero();
}

EllipticCylinderSource::EllipticCylinderSource(const VectorR3 &p, double r1,
                                               double r2, VectorR3 &L,
                                               double act) :
    Source(p, act),
    radius1(r1),
    radius2(r2)
{
    SetAxis(L);
}

VectorR3 EllipticCylinderSource::Decay() {
    double r1sq = radius1*radius1;
    double r2sq = radius2*radius2;

    VectorR3 positron;
    do {
        positron.x = (1.0 - 2.0*Random::Uniform())*radius1;
        positron.y = (1.0 - 2.0*Random::Uniform())*radius2;
        positron.z = 0;
    } while (positron.x*positron.x/r1sq + positron.y*positron.y/r2sq > 1);
    positron.z = length*(0.5 - Random::Uniform());
    return(local_to_global * positron);
}

void EllipticCylinderSource::SetRadius(double r1, double r2)
{
    radius1 = r1;
    radius2 = r2;
}

void EllipticCylinderSource::SetAxis(VectorR3 &L)
{
    length = L.Norm();
    axis = L.MakeUnit();
    local_to_global = RefAxisPlusTransToMap(axis, position);
    global_to_local = local_to_global.Inverse();
}

bool EllipticCylinderSource::Inside(const VectorR3 & pos) const
{
    const VectorR3 roted = global_to_local * pos;
    const double r1 = (roted.x * roted.x) / (radius1 * radius1);
    const double r2 = (roted.y * roted.y) / (radius2 * radius2);
    return (((r1 + r2) <= 1.0) && (std::abs(roted.z) <= (length / 2.0)));
}
