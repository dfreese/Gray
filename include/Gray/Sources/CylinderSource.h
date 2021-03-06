/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef CYLINDERSOURCE_H_
#define CYLINDERSOURCE_H_

#include "Gray/VrMath/MathMisc.h"
#include "Gray/VrMath/LinearR3.h"
#include "Gray/Sources/Source.h"

class CylinderSource : public Source
{
public:
    CylinderSource() = default;
    CylinderSource(
            const VectorR3& position,
            double radius,
            double height,
            const VectorR3& axis,
            double act);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;

private:
    double radius = 1.0;
    double height = 1.0;
    VectorR3 axis = {0.0, 0.0, 1.0};
    RigidMapR3 local_to_global;
    RigidMapR3 global_to_local;
};

#endif /*CYLINDERSOURCE_H_*/
