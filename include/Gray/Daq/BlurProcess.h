/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef blurprocess_h
#define blurprocess_h

#include <functional>
#include "Gray/Daq/Process.h"

struct ProcessStats;

class BlurProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using BlurF = std::function<void(EventT&)>;

    BlurProcess(BlurF blurring_func);
    EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const final;

private:
    /*!
     * A function that somehow blurs the event
     */
    BlurF blur_func;
};
#endif /* blurprocess_h */
