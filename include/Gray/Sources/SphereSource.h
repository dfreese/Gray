/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef SPHERESOURCE_H
#define SPHERESOURCE_H

#include "Gray/Sources/Source.h"

class SphereSource : public Source
{
public:
    SphereSource();
    SphereSource(const VectorR3 &pos, double radius, double act);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;
private:
    double radius;
};

#endif // SPHERESOURCE_H
