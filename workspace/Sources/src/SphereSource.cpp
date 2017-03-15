#include <Sources/SphereSource.h>
#include <Random/Random.h>

SphereSource::SphereSource()
{
    position.SetZero();
    radius = 1.0;
}

SphereSource::SphereSource(const VectorR3 &p, double rad, double act)
{
    position = p;
    SetActivity(act);
    radius = rad;
}

VectorR3 SphereSource::Decay(unsigned int photon_number, double time)
{
    if (isotope == NULL) {
        return(VectorR3(0,0,0));
    }

    VectorR3 pos;
    do {
        pos.x = (1.0 - 2.0*Random::Uniform());
        pos.y = (1.0 - 2.0*Random::Uniform());
        pos.z = (1.0 - 2.0*Random::Uniform());
    } while (pos.Norm() > 1.0);

    
    pos *= radius;
    pos += position;
    isotope->Decay(photon_number, time, pos);
    return(pos);
}

void SphereSource::SetRadius(double r)
{
    radius = r;
}

bool SphereSource::Inside(const VectorR3 & pos) const
{
    if (isotope == NULL) {
        return false;
    }

    VectorR3 dist;
    dist = pos;
    dist -= position;
    if (dist.Norm() < radius) {
        return true;
    } else {
        return false;
    }
}
