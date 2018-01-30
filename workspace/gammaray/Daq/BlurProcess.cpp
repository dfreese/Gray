/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Daq/BlurProcess.h>
#include <Daq/ProcessStats.h>

/*!
 *
 */
BlurProcess::BlurProcess(BlurF blurring_func) :
    blur_func(blurring_func)
{
}

/*!
 *
 */
BlurProcess::EventIter BlurProcess::process(
        EventIter begin, EventIter end, ProcessStats& stats) const
{
    for (auto iter = begin; iter != end; ++iter) {
        EventT & event = *iter;
        if (!event.dropped) {
            stats.no_kept++;
            blur_func(event);
        }
    }
    return (end);
}
