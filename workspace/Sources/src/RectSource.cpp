#include <Sources/RectSource.h>
#include <Random/Random.h>

using namespace std;

RectSource::RectSource()
{
    position.SetZero();
}

RectSource::RectSource(const VectorR3 &p, const VectorR3 &sz, double act)
{
    SetPosition(p);
    SetSize(sz);
    SetActivity(act);
}

void RectSource::Decay(unsigned int photon_number, double time)
{
    if (isotope == NULL) {
        return;
    }
    VectorR3 pos;
    pos.x = (0.5 - Random::Uniform());
    pos.y = (0.5 - Random::Uniform());
    pos.z = (0.5 - Random::Uniform());
    pos.ArrayProd(size);
    pos += position;
    isotope->SetPosition(pos);
    isotope->Decay(photon_number, time);
}

void RectSource::SetSize(const VectorR3 &sz)
{
    size = sz;
}

bool RectSource::Inside(const VectorR3 & pos) const
{

    // FIXME: Source rotation does not work
    // (translation works)
    if (isotope == NULL) {
        return false;
    }

    VectorR3 dist;
    dist = pos;
    dist -= position;

    if (fabs(dist.x) > size.x/2.0) {
        return false;
    }
    if (fabs(dist.y) > size.y/2.0) {
        return false;
    }
    if (fabs(dist.z) > size.z/2.0) {
        return false;
    }

    return true;
}
