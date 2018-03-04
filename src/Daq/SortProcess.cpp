/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Daq/SortProcess.h"
#include <algorithm>
#include <iterator>
#include "Gray/Daq/ProcessStats.h"

/*!
 *
 */
SortProcess::SortProcess(TimeT max_time_to_wait) :
    max_wait_time(max_time_to_wait)
{
}

namespace {
/*!
 * Taken from my answer here:
 * https://codereview.stackexchange.com/a/160363/135154
 *
 * Since we know that our sorting is done on mostly sorted data, insertion
 * sort will be faster, and is stable so it's not swapping elements around.
 */
template<class I, class C = std::less<typename std::iterator_traits<I>::value_type>>
void insertion_sort(I begin, I end, C comp = C()) {
    for (auto i = begin; i != end; ++i) {
        auto index = std::upper_bound(begin, i, *i, comp);
        std::rotate(index, i, std::next(i));
    }
}
}

/*!
 *
 */
SortProcess::EventIter SortProcess::process(
        EventIter begin, EventIter end,
        ProcessStats& stats) const
{
    // The timeout detection in this function requires a non-empty container
    // so if we're given an empty range, bail right away.
    if (begin == end) {
        return(end);
    }

    auto time_cmp = [](const EventT& e0, const EventT& e1) {
        return(e0.time < e1.time);
    };
    insertion_sort(begin, end, time_cmp);

    // work back from the end to figure out where we've timed out, so what
    // we will consider sorted.
    auto timed_out = std::prev(end);
    const TimeT out_time = (*timed_out).time - max_wait_time;
    for (; timed_out != begin; --timed_out) {
        if ((*timed_out).time <= out_time) {
            break;
        }
    }
    // TODO: implement this in the sort loop so we don't need to do go over
    // the events twice.
    stats.no_kept += std::count_if(
            begin, timed_out, [](const EventT& e) { return (!e.dropped); });

    return (timed_out);
};
