/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

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
