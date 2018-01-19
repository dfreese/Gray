/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef FilterFunctors_h
#define FilterFunctors_h
#include <Daq/Process.h>

namespace FilterFunctors {
    using EventT = Process::EventT;
    using DetIdT = Process::DetIdT;

    struct FilterEnergyGateLow {
        FilterEnergyGateLow(double energy_val);
        bool operator() (EventT & event) const;
        const double value;
    };

    struct FilterEnergyGateHigh {
        FilterEnergyGateHigh(double energy_val);
        bool operator() (EventT & event) const;
        const double value;
    };

}

#endif // FilterFunctors_h
