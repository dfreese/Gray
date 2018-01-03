#include <Sources/SphereSource.h>
#include <Random/Random.h>

SphereSource::SphereSource() :
    Source(),
    radius(1.0)
{
}

SphereSource::SphereSource(const VectorR3 &p, double rad, double act) :
    Source(p, act),
    radius(rad)
{
}

VectorR3 SphereSource::Decay() const {
    return(Random::UniformSphereFilled() * radius + position);
}

bool SphereSource::Inside(const VectorR3 & pos) const
{
    return ((pos - position).Norm() < radius);
}
