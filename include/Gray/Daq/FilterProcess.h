/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef filterprocess_h
#define filterprocess_h

#include <functional>
#include "Gray/Daq/Process.h"

struct ProcessStats;

class FilterProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using FilterF = std::function<bool(EventT&)>;

    FilterProcess(FilterF filter_func);
    EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const final;

private:
    /*!
     * A function that returns true if the event should be kept.
     */
    FilterF filt_func;
};
#endif /* filterprocess_h */
