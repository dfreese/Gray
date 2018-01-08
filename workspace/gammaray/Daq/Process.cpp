#include <Daq/Process.h>
#include <algorithm>
#include <Daq/ProcessStats.h>

void Process::stop(EventIter begin, EventIter end, ProcessStats& stats) const {
    // Most processes don't do anything further on the data.  Count up the
    // number of events that are not dropped past the return.
    auto ready = process(begin, end, stats);
    stats.no_kept += std::count_if(
            ready, end, [](const EventT& e) { return (!e.dropped); });
}

