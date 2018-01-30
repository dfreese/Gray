/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef RECTSOURCE_H
#define RECTSOURCE_H

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Sources/Source.h>


class RectSource : public Source
{
public:
    RectSource();
    RectSource(const VectorR3 &pos, const VectorR3 &sz,
               const VectorR3 & orientation, double act);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;
private:
    const VectorR3 size;
    const RigidMapR3 local_to_global;
    const RigidMapR3 global_to_local;
};

#endif /*RECTSOURCE_H*/
