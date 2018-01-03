#include <Sources/PointSource.h>

PointSource::PointSource(const VectorR3 &p, double act) :
    Source(p, act)
{
}

VectorR3 PointSource::Decay() {
    return(position);
}

bool PointSource::Inside(const VectorR3 & pos) const
{
    return false;
}
