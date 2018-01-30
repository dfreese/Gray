/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <algorithm>
#include <iterator>
#include <Daq/DeadtimeProcess.h>
#include <Daq/ProcessStats.h>

/*!
 *
 */
DeadtimeProcess::DeadtimeProcess(const IdLookupT& lookup,
                                 TimeT deadtime, bool paralyzable) :
    id_lookup(lookup),
    time_window(deadtime),
    is_paralyzable(paralyzable)
{
}

/*!
 *
 */
DeadtimeProcess::EventIter DeadtimeProcess::process(
        EventIter begin, EventIter end,
        ProcessStats& stats) const
{
    auto current_event = begin;
    for (; current_event != end; current_event++) {
        if ((*current_event).dropped) {
            continue;
        }

        const DetIdT current_event_id = mapped_id(*current_event);
        // Check to see where this event times out
        TimeT window = (*current_event).time + time_window;
        auto next_event = std::next(current_event);
        for (; next_event != end; ++next_event) {
            if ((*next_event).dropped) {
                continue;
            }

            const TimeT next_time = (*next_event).time;
            if (next_time >= window) {
                stats.no_kept++;
                break;
            }
            if (current_event_id == mapped_id(*next_event)) {
                (*next_event).dropped = true;
                stats.no_dropped++;
                if (is_paralyzable) {
                    window = next_time + time_window;
                }
            }
        }
        if (next_event == end) {
            return (current_event);
        }
    }
    // Should never hit here.
    return (current_event);
}

/*!
 *
 */
DeadtimeProcess::DetIdT DeadtimeProcess::mapped_id(const EventT& event) const {
    return(id_lookup[event.det_id]);
}
