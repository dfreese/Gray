#include "SphereSource.h"

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

void SphereSource::Decay(unsigned int photon_number)
{
    if (isotope == NULL) {
        return;
    }

    VectorR3 pos;
    do {
        pos.x = (1.0 - 2.0*Random());
        pos.y = (1.0 - 2.0*Random());
        pos.z = (1.0 - 2.0*Random());
    } while (pos.Norm() > 1.0);

    pos *= radius;
    pos += position;
    isotope->SetMaterial(GetMaterial());
    isotope->SetPosition(pos);
    isotope->Decay(photon_number);
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
