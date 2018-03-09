/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Sources/AnnulusCylinderSource.h"
#include "Gray/Random/Random.h"

AnnulusCylinderSource::AnnulusCylinderSource(
        const VectorR3& position,
        double radius, double height,
        const VectorR3& axis, double activity) :
    Source(position, activity),
    radius(radius),
    height(height),
    local_to_global(RefAxisPlusTransToMap(axis, position))
{
}

VectorR3 AnnulusCylinderSource::Decay() const {
    return(local_to_global * Random::UniformAnnulusCylinder(height, radius));
}

bool AnnulusCylinderSource::Inside(const VectorR3&) const {
    // Nothing can be inside of an Annulus which is infinitely thin.
    return (false);
}
