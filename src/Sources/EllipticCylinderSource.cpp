/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Sources/EllipticCylinderSource.h"
#include "Gray/Random/Random.h"

EllipticCylinderSource::EllipticCylinderSource(
        const VectorR3& position,
        double radius1, double radius2, double height,
        const VectorR3& axis,
        double activity) :
    Source(position, activity),
    radius1(radius1),
    radius2(radius2),
    height(height),
    axis(axis),
    local_to_global(RefAxisPlusTransToMap(axis, position)),
    global_to_local(local_to_global.Inverse())
{
}

VectorR3 EllipticCylinderSource::Decay() const {
    double r1sq = radius1*radius1;
    double r2sq = radius2*radius2;

    VectorR3 positron;
    do {
        positron.x = (1.0 - 2.0*Random::Uniform())*radius1;
        positron.y = (1.0 - 2.0*Random::Uniform())*radius2;
        positron.z = 0;
    } while (positron.x*positron.x/r1sq + positron.y*positron.y/r2sq > 1);
    positron.z = height * (0.5 - Random::Uniform());
    return(local_to_global * positron);
}

bool EllipticCylinderSource::Inside(const VectorR3 & pos) const
{
    const VectorR3 roted = global_to_local * pos;
    const double r1 = (roted.x * roted.x) / (radius1 * radius1);
    const double r2 = (roted.y * roted.y) / (radius2 * radius2);
    return (((r1 + r2) <= 1.0) && (std::abs(roted.z) <= (height / 2.0)));
}
