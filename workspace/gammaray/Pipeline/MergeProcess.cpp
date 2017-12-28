#include <algorithm>
#include <Pipeline/MergeProcess.h>

/*!
 *
 */
MergeProcess::MergeProcess(const std::vector<DetIdT> & lookup,
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
        const TimeT window = get_time(cur_event) + time_window;
        auto next_iter = cur_iter + 1;
        for (; next_iter != end; next_iter++) {
            EventT & next_event = *next_iter;
            if (next_event.dropped) {
                continue;
            }
            if (get_time(next_event) >= window) {
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
    return(id_lookup[get_det_id(event)]);
}
