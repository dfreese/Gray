/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Daq/FilterFunctors.h"

namespace FilterFunctors {
FilterEnergyGateLow::FilterEnergyGateLow(double energy_val) :
    value(energy_val)
{
}

bool FilterEnergyGateLow::operator() (EventT & event) const {
    bool val = event.energy >= value;
    if (!val) {
        event.dropped = true;
    }
    return(val);
}

FilterEnergyGateHigh::FilterEnergyGateHigh(double energy_val) :
    value(energy_val)
{
}

bool FilterEnergyGateHigh::operator() (EventT & event) const {
    bool val = event.energy <= value;
    if (!val) {
        event.dropped = true;
    }
    return(val);
}
}
