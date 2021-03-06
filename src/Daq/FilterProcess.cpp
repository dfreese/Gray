/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Daq/FilterProcess.h"
#include "Gray/Daq/ProcessStats.h"

/*!
 *
 */
FilterProcess::FilterProcess(FilterF filter_func) :
    filt_func(filter_func)
{
}

/*!
 *
 */
FilterProcess::EventIter FilterProcess::process(
        EventIter begin, EventIter end,
        ProcessStats& stats) const
{
    for (auto iter = begin; iter != end; ++iter) {
        EventT & event = *iter;
        if (!event.dropped) {
            if (this->filt_func(event)) {
                stats.no_kept++;
            } else {
                stats.no_dropped++;
            }
        }
    }
    return (end);
};
