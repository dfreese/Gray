#include <Sources/RectSource.h>
#include <Random/Random.h>

using namespace std;

RectSource::RectSource():
    Source(),
    size(0, 0, 0)
{
}

RectSource::RectSource(const VectorR3 &p, const VectorR3 &sz,
                       const RigidMapR3 & map, double act) :
    Source(p, act),
    size(sz),
    mapping(map),
    inv_map(map.Inverse())
{
}

VectorR3 RectSource::Decay(int photon_number, double time)
{
    VectorR3 pos;
    pos.x = (0.5 - Random::Uniform());
    pos.y = (0.5 - Random::Uniform());
    pos.z = (0.5 - Random::Uniform());
    mapping.Transform(&pos);
    isotope->Decay(photon_number, time, source_num, pos);
    return(pos);
}

void RectSource::SetSize(const VectorR3 &sz)
{
    size = sz;
}

bool RectSource::Inside(const VectorR3 & pos) const
{
    VectorR3 dist;
    inv_map.Transform(pos, &dist);

    if ((abs(dist.x) > size.x/2.0) || (abs(dist.y) > size.y/2.0) ||
        (abs(dist.z) > size.z/2.0))
    {
        return false;
    }
    return true;
}
