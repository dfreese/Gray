/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef processor_h
#define processor_h

#include <vector>
#include <Physics/Interaction.h>

class ProcessStats;

class Process {
public:
    using EventT = Interaction;
    using ContainerT = std::vector<EventT>;
    using EventIter = ContainerT::iterator;
    using TimeT = decltype(EventT::time);
    using DetIdT = decltype(EventT::det_id);

    Process() = default;
    virtual ~Process() = default;

    virtual EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const = 0;
    virtual void stop(
            EventIter begin, EventIter end,
            ProcessStats& stats) const;
};

#endif /* processor_h */
