/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Sources/PointSource.h"

PointSource::PointSource(const VectorR3 &p, double act) :
    Source(p, act)
{
}

VectorR3 PointSource::Decay() const {
    return(position);
}

bool PointSource::Inside(const VectorR3&) const {
    return false;
}
