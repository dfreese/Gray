/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef ANNULUSELLIPTICCYLINDERSOURCE_H_
#define ANNULUSELLIPTICCYLINDERSOURCE_H_

#include <vector>
#include "Gray/VrMath/MathMisc.h"
#include "Gray/VrMath/LinearR3.h"
#include "Gray/Sources/Source.h"

class AnnulusEllipticCylinderSource : public Source
{
public:
    AnnulusEllipticCylinderSource(
            const VectorR3& pos,
            double radius1, double radius2, double height,
            const VectorR3& axis, double act);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;
    void SetRadius(double r1, double r2);
    void SetAxis(VectorR3 L);
    static double EllipticE(double m);
    double InverseEllipticE(double arc_length) const;
    static double EllipticK(double m);
    static double IncompleteEllipticE(double phi, double m);
    static std::vector<double> GenerateTable(double radius1, double radius2);

private:
    double radius1;
    double radius2;
    double height;
    VectorR3 axis;
    RigidMapR3 local_to_global;
    RigidMapR3 global_to_local;
    std::vector<double> circ;
};

#endif /*ANNULUSELLIPTICCYLINDERSOURCE_H_*/
