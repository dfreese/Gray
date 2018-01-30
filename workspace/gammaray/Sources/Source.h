/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef SOURCE_H
#define SOURCE_H

#include <memory>

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>
#include <Physics/Isotope.h>
#include <Physics/Physics.h>

class Source
{
public:
    Source() :
        isotope(nullptr),
        activity(0),
        negative(false),
        source_num(0),
        position(0, 0, 0)
    {
    }

    Source(const VectorR3 & pos, double activity_ucui) :
        isotope(nullptr),
        activity(activity_ucui),
        negative(false),
        source_num(0),
        position(pos)
    {
        if (activity < 0.0) {
            negative = true;
        } else {
            activity *= Physics::decays_per_microcurie;
            negative = false;
        }
    }
    virtual ~Source() {}

    /*!
     * Returns the initial activity of the source.
     */
    double GetActivity() const {
        return activity;
    }

    /*!
     * Calculates the activity of the source given the isotope's half-life.
     */
    double GetActivity(double time) const {
        return (activity * isotope->FractionRemaining(time));
    }

    double GetExpectedDecays(double start, double time) const {
        return (activity * isotope->FractionIntegral(start, time));
    }

    double GetExpectedPhotons(double start, double time) const {
        return (isotope->ExpectedNoPhotons() * GetExpectedDecays(start, time));
    }

    void SetIsotope(std::shared_ptr<const Isotope> i)
    {
        isotope = std::move(i);
    }

    void SetPosition(const VectorR3 & pos)
    {
        position = pos;
    }

    VectorR3 GetPosition() const {
        return (position);
    }

    void SetSourceNum(int i)
    {
        source_num = i;
    }

    bool isNegative() const {
        return negative;
    }

    const Isotope& GetIsotope() const
    {
        return *isotope.get();
    }

    virtual bool Inside(const VectorR3 &pos) const = 0;
    virtual VectorR3 Decay() const = 0;

protected:
    std::shared_ptr<const Isotope> isotope;
    double activity;
    bool negative;
    int source_num;
    VectorR3 position;
};

#endif // SOURCE_H
