/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef BlurFunctors_H
#define BlurFunctors_H
#include "Gray/Daq/Process.h"

namespace BlurFunctors {
    using EventT = Process::EventT;
    using DetIdT = Process::DetIdT;

    struct BlurEnergy {
        BlurEnergy(double fwhm_percent);
        void operator() (EventT & event) const;
        const double value;
    };


    struct BlurEnergyReferenced {
        BlurEnergyReferenced(double fwhm_percent, double ref_energy);
        void operator() (EventT & event) const;
        const double value;
        const double ref;
    };

    struct BlurTime {
        BlurTime(double fwhm_time, double max_blur);
        void operator() (EventT & event) const;
        const double value;
        const double max;
    };
}

#endif // BlurFunctors_H
