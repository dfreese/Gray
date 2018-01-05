#include <algorithm>
#include <iterator>
#include <Daq/MergeProcess.h>

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
MergeProcess::EventIter MergeProcess::_process_events(EventIter begin,
                                                      EventIter end)
{
    auto cur_iter = begin;
    for (; cur_iter != end; cur_iter++) {
        EventT & cur_event = *cur_iter;
        if (cur_event.dropped) {
            continue;
        }
        const int current_event_id = mapped_id(cur_event);
        // Check to see where this event times out
        const TimeT window = cur_event.time + time_window;
        auto next_iter = std::next(cur_iter);
        for (; next_iter != end; ++next_iter) {
            EventT & next_event = *next_iter;
            if (next_event.dropped) {
                continue;
            }
            if (next_event.time >= window) {
                break;
            }
            if (current_event_id == mapped_id(next_event)) {
                merge_func(cur_event, next_event);
                this->inc_no_dropped();
            }
        }
        if (next_iter == end) {
            return (cur_iter);
        }
    }
    // shouldn't happen
    return (cur_iter);
};

/*!
 *
 */
void MergeProcess::_stop(EventIter begin, EventIter end) {
    _process_events(begin, end);
};

/*!
 *
 */
void MergeProcess::_reset() {
}

/*!
 *
 */
MergeProcess::DetIdT MergeProcess::mapped_id(const EventT& event) const {
    return(id_lookup[event.det_id]);
}
