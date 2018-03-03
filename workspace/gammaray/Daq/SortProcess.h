/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef sortprocess_h
#define sortprocess_h

#include <Daq/Process.h>

struct ProcessStats;

class SortProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;

    SortProcess(TimeT max_time_to_wait);
    EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const final;

private:
    TimeT max_wait_time;
};
#endif // sortprocess_h
