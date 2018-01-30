/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Sources/RectSource.h>
#include <Random/Random.h>

using namespace std;

RectSource::RectSource():
    Source(),
    size(0, 0, 0),
    local_to_global(RefAxisPlusTransToMap(VectorR3(0, 0, 1), VectorR3(0, 0, 0))),
    global_to_local(local_to_global.Inverse())
{
}

RectSource::RectSource(const VectorR3 &p, const VectorR3 &sz,
                       const VectorR3 & orientation, double act) :
    Source(p, act),
    size(sz),
    local_to_global(RefAxisPlusTransToMap(orientation, p)),
    global_to_local(local_to_global.Inverse())
{
}

VectorR3 RectSource::Decay() const {
    return (local_to_global * Random::UniformRectangle(size));
}

bool RectSource::Inside(const VectorR3 & pos) const
{
    const VectorR3 dist = global_to_local * pos;

    return ((std::abs(dist.x) <= size.x / 2.0) &&
            (std::abs(dist.y) <= size.y / 2.0) &&
            (std::abs(dist.z) <= size.z / 2.0));
}
