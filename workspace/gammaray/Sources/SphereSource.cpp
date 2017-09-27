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

VectorR3 SphereSource::Decay(int photon_number, double time)
{
    VectorR3 pos = Random::UniformSphereFilled() * radius + position;
    isotope->Decay(photon_number, time, source_num, pos);
    return(pos);
}

void SphereSource::SetRadius(double r)
{
    radius = r;
}

bool SphereSource::Inside(const VectorR3 & pos) const
{
    return ((pos - position).Norm() < radius);
}
