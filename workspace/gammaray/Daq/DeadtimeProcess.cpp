
//
//  deadtimeprocess.h
//  graypipeline
//
//  Created by David Freese on 4/17/17.
//
//
#include <algorithm>
#include <iterator>
#include <Daq/DeadtimeProcess.h>

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
DeadtimeProcess::EventIter DeadtimeProcess::_process_events(EventIter begin,
                                                            EventIter end)
{
    auto current_event = begin;
    for (; current_event != end; current_event++) {
        if ((*current_event).dropped) {
            continue;
        }
        const DetIdT current_event_id = mapped_id(*current_event);
        // Check to see where this event times out
        TimeT window = get_time(*current_event) + time_window;
        auto next_event = std::next(current_event);
        for (; next_event != end; ++next_event) {
            if ((*next_event).dropped) {
                continue;
            }

            const TimeT next_time = get_time(*next_event);
            if (next_time >= window) {
                break;
            }
            if (current_event_id == mapped_id(*next_event)) {
                (*next_event).dropped = true;
                this->inc_no_dropped();
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
};

/*!
 *
 */
void DeadtimeProcess::_stop(EventIter begin, EventIter end) {
    _process_events(begin, end);
};

/*!
 *
 */
void DeadtimeProcess::_reset() {
}

/*!
 *
 */
DeadtimeProcess::DetIdT DeadtimeProcess::mapped_id(const EventT& event) const {
    return(id_lookup[get_det_id(event)]);
}
