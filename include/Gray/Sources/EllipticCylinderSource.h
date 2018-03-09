/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef ELLIPTICCYLINDERSOURCE_H_
#define ELLIPTICCYLINDERSOURCE_H_

#include "Gray/VrMath/MathMisc.h"
#include "Gray/VrMath/LinearR3.h"
#include "Gray/Sources/Source.h"

class EllipticCylinderSource : public Source
{
public:
    EllipticCylinderSource() = default;
    EllipticCylinderSource(
            const VectorR3 &pos,
            double radius1, double radius2, double height,
            const VectorR3& axis, double act);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;

private:
    double radius1 = 1.0;
    double radius2 = 1.0;
    double height = 1.0;
    VectorR3 axis = {0.0, 0.0, 1.0};
    RigidMapR3 local_to_global;
    RigidMapR3 global_to_local;
};

#endif /*ELLIPTICCYLINDERSOURCE_H_*/
