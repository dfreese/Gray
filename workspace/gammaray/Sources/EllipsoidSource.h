/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef ELLIPSOIDSOURCE_H_
#define ELLIPSOIDSOURCE_H_

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Sources/Source.h>

using namespace std;

class EllipsoidSource : public Source
{
public:
    EllipsoidSource();
    EllipsoidSource(const VectorR3 &center, const VectorR3 &a1, const VectorR3 &a2, double r1, double r2, double r3, double act);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;
    void SetRadius(double r1, double r2, double r3);
    void SetAxis(const VectorR3 &a1,const VectorR3 &a2);
private:
    double radius1, radius2, radius3;
    VectorR3 axis;
    VectorR3 axis1;
    VectorR3 axis2;
    VectorR3 axis3;
    RigidMapR3 local_to_global;
    RigidMapR3 global_to_local;
};

#endif /*ELLIPSOIDSOURCE_H_*/
