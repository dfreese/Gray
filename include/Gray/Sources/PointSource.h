/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef POINTSOURCE_H_
#define POINTSOURCE_H_

#include "Gray/Sources/Source.h"

class PointSource : public Source
{
public:
    PointSource(const VectorR3 &p, double act);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;
};

#endif // POINTSOURCE_H_
