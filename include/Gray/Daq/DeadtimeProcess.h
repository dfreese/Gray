/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef deadtimeprocess_h
#define deadtimeprocess_h

#include <vector>
#include "Gray/Daq/Mapping.h"
#include "Gray/Daq/Process.h"

struct ProcessStats;

class DeadtimeProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;
    using DetIdT = Process::DetIdT;
    using IdLookupT = Mapping::IdLookupT;

    DeadtimeProcess(const IdLookupT& lookup, TimeT deadtime,
                    bool paralyzable);
    EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const final;

private:
    DetIdT mapped_id(const EventT& event) const;

    /*!
     * A lookup table for the component id that is associated with each
     * detector id.
     */
    const IdLookupT id_lookup;
    TimeT time_window;
    bool is_paralyzable;
};
#endif // deadtimeprocess_h
