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

VectorR3 RectSource::Decay(int photon_number, double time)
{
    if (isotope == NULL) {
        return(VectorR3(0,0,0));
    }
    VectorR3 pos;
    pos.x = (0.5 - Random::Uniform());
    pos.y = (0.5 - Random::Uniform());
    pos.z = (0.5 - Random::Uniform());
    pos.ArrayProd(size);
    pos += position;
    isotope->Decay(photon_number, time, source_num, pos);
    return(pos);
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
