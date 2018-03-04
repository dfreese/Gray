/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef ANNULUSCYLINDERSOURCE_H_
#define ANNULUSCYLINDERSOURCE_H_

#include "Gray/VrMath/LinearR3.h"
#include "Gray/Sources/Source.h"

class AnnulusCylinderSource : public Source
{
public:
    AnnulusCylinderSource();
    AnnulusCylinderSource(const VectorR3 & position, double radius, VectorR3 L,
                          double activity);
    VectorR3 Decay() const override;
    bool Inside(const VectorR3 & pos) const override;

private:
    double radius;
    double length;
    RigidMapR3 local_to_global;
};

#endif /*ANNULUSCYLINDERSOURCE_H_*/
