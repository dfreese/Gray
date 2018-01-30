/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef ELLIPTICCYLINDERSOURCE_H_
#define ELLIPTICCYLINDERSOURCE_H_

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Sources/Source.h>

class EllipticCylinderSource : public Source
{
public:
    EllipticCylinderSource();
    EllipticCylinderSource(const VectorR3 &pos, double radius1, double radius2, VectorR3 &L, double act);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;
    void SetRadius(double r1, double r2);
    void SetAxis(VectorR3 &L);

private:
    double radius1;
    double radius2;
    double length;
    VectorR3 axis;
    RigidMapR3 local_to_global;
    RigidMapR3 global_to_local;
};

#endif /*ELLIPTICCYLINDERSOURCE_H_*/
