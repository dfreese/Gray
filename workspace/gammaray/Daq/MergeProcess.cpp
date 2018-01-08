#include <algorithm>
#include <iterator>
#include <Daq/MergeProcess.h>
#include <Daq/ProcessStats.h>

/*!
 *
 */
MergeProcess::MergeProcess(const IdLookupT& lookup,
                           TimeT t_window,
                           MergeF merge_fc) :
    id_lookup(lookup),
    time_window(t_window),
    merge_func(merge_fc)
{
}

/*!
 *
 */
MergeProcess::EventIter MergeProcess::process(
        EventIter begin, EventIter end,
        ProcessStats& stats) const
{
    auto cur_iter = begin;
    for (; cur_iter != end; cur_iter++) {
        EventT & cur_event = *cur_iter;
        if (cur_event.dropped) {
            continue;
        }
        const DetIdT current_event_id = mapped_id(cur_event);
        // Check to see where this event times out
        const TimeT window = cur_event.time + time_window;
        auto next_iter = std::next(cur_iter);
        for (; next_iter != end; ++next_iter) {
            EventT & next_event = *next_iter;
            if (next_event.dropped) {
                continue;
            }
            if (next_event.time >= window) {
                // We have found an event that is outside of the window so we
                // know that cur_event will be kept, and not be merged with
                // something else.
                stats.no_kept++;
                break;
            }
            if (current_event_id == mapped_id(next_event)) {
                merge_func(cur_event, next_event);
                stats.no_dropped++;
                // merge_func can drop either cur_event or next_event, so in
                // the case that the earlier event is dropped, then bail out
                // of this loop and pick up next_event as the new cur_event.
                if (cur_event.dropped) {
                    break;
                }
            }
        }
        if (next_iter == end) {
            return (cur_iter);
        }
    }
    // should only happen on an empty range call (begin == end)
    return (cur_iter);
};

/*!
 *
 */
MergeProcess::DetIdT MergeProcess::mapped_id(const EventT& event) const {
    return(id_lookup[event.det_id]);
}
