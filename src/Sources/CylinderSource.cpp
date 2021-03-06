/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Sources/CylinderSource.h"
#include "Gray/Random/Random.h"

CylinderSource::CylinderSource(
        const VectorR3& position,
        double radius, double height,
        const VectorR3& axis, double act) :
    Source(position, act),
    radius(radius),
    height(height),
    axis(axis),
    local_to_global(RefAxisPlusTransToMap(axis, position)),
    global_to_local(local_to_global.Inverse())
{
}

VectorR3 CylinderSource::Decay() const {
    return(local_to_global * Random::UniformCylinder(height, radius));
}

bool CylinderSource::Inside(const VectorR3 & pos) const
{
    // TODO: refactor this out for all cylinders.
    const VectorR3 roted = global_to_local * pos;
    if ((roted.x * roted.x + roted.y * roted.y) > radius * radius) {
        return false;
    }
    if (std::abs(roted.z) > height/2.0) {
        return false;
    }
    return true;
}
