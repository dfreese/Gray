#include <algorithm>
#include <Daq/CoincProcess.h>

/*!
 *
 */
CoincProcess::CoincProcess(TimeT coinc_win, bool reject_multiple_events,
                           bool is_paralyzable, TimeT win_offset) :
    coinc_window(coinc_win),
    window_offset(win_offset),
    reject_multiples(reject_multiple_events),
    paralyzable(is_paralyzable),
    no_coinc_pair_events(0),
    no_coinc_multiples_events(0),
    no_coinc_single_events(0),
    no_coinc_events(0)
{

}

/*!
 *
 */
long CoincProcess::get_no_coinc_events() const {
    return(no_coinc_events);
}

/*!
 *
 */
long CoincProcess::get_no_coinc_pair_events() const {
    return(no_coinc_pair_events);
}

/*!
 *
 */
long CoincProcess::get_no_coinc_multiples_events() const {
    return(no_coinc_multiples_events);
}

/*!
 *
 */
long CoincProcess::get_no_coinc_singles() const {
    return(no_coinc_single_events);
}

/*!
 *
 */
std::ostream& operator<< (std::ostream & os, const CoincProcess & cp)
{
    os << "coinc events            : " << cp.no_coinc_events << "\n"
       << "events in coinc pair    : " << cp.no_coinc_pair_events << "\n"
       << "events in coinc multiple: " << cp.no_coinc_multiples_events << "\n"
       << "events in coinc single  : " << cp.no_coinc_single_events << "\n";
    return(os);
}

void CoincProcess::_reset() {
    no_coinc_pair_events = 0;
    no_coinc_multiples_events = 0;
    no_coinc_single_events = 0;
    no_coinc_events = 0;
}

/*!
 *
 */
CoincProcess::EventIter CoincProcess::_process_events(EventIter begin,
                                                      EventIter end)
{
    return(process_events_optional_stop(begin, end, false));
}

/*!
 *
 */
void CoincProcess::_stop(EventIter begin, EventIter end) {
    process_events_optional_stop(begin, end, true);
}

/*!
 *
 */
CoincProcess::EventIter CoincProcess::process_events_optional_stop(
        EventIter begin, EventIter end, bool stopping)
{
    // coinc_id == -1 means event hasn't been touched.  coinc_id == -2
    // indicateds a rejected event.  Zero or higher means it has been
    // accepted.  coinc_id can be overwritten by other coinc processes,
    // so this has to be redone for all.
    for (auto iter = begin; iter != end; ++iter) {
        (*iter).coinc_id = -1;
    }
    // We hold onto cur_iter as a way of pointing to where we'd pickup next
    // time.  This will be end if all of the events are timedout or stopping
    // is true.  Otherwise, the next call to this function should use
    // cur_iter as begin.
    EventIter cur_iter = begin;
    for (;cur_iter != end; ++cur_iter) {
        EventT & current_event = *cur_iter;
        if (current_event.dropped || (current_event.coinc_id != -1)) {
            continue;
        }

        // Find an iterator pointing to the first event at or after the
        // start of the window.
        const TimeT window_start = window_offset;
        // We require the window iterator to start after cur_iter.  If we
        // were not dealing with delayed windows, then we could assume that
        // the window and the current iterator formed one contiguous block
        // but that's not the case here.
        EventIter window_start_iter = cur_iter + 1;
        for (; window_start_iter != end; window_start_iter++) {
            EventT & window_start_event = *window_start_iter;
            if (window_start_event.dropped ||
                (window_start_event.coinc_id != -1))
            {
                continue;
            }
            TimeT delta_t = time_diff(window_start_event, current_event);
            if (delta_t >= window_start) {
                break;
            }
        }

        // Look for the end of the window.  We start by looking at
        // window_start_iter.  window_start_iter and window_end_iter can
        // be the same.  This indicates there are no events in the window
        // for current_event.
        //
        // We leave window_end non_const as a paralyzable window can extend
        // this outward.
        TimeT window_end = window_offset + coinc_window;
        EventIter window_end_iter = window_start_iter;
        for (; window_end_iter != end; window_end_iter++) {
            EventT & window_end_event = *window_end_iter;
            if (window_end_event.dropped ||
                (window_end_event.coinc_id != -1))
            {
                continue;
            }
            TimeT delta_t = time_diff(window_end_event, current_event);
            if (delta_t >= window_end) {
                break;
            } else {
                if (paralyzable) {
                    window_end = delta_t + coinc_window;
                }
            }
        }

        if ((window_end_iter == end) && (!stopping)) {
            break;
        }

        // Find the number of non-dropped events pointed to by the window.
        int no_events = 1;
        for (auto iter = window_start_iter; iter != window_end_iter; ++iter)  {
            EventT & event = *iter;
            if (!event.dropped) {
                no_events++;
            }
        }

        // Sort out the singles, doubles, and multiples.
        bool keep_events = false;
        if (no_events == 2) {
            no_coinc_pair_events += no_events;
            keep_events = true;
        } else if (no_events > 2) {
            no_coinc_multiples_events += no_events;
            keep_events = !reject_multiples;
        } else {
            no_coinc_single_events += no_events;
        }
        for (auto iter = window_start_iter; iter != window_end_iter; ++iter)  {
            EventT & event = *iter;
            if (event.dropped) {
                continue;
            }
            if (keep_events) {
                event.coinc_id = no_coinc_events;
            } else {
                event.coinc_id = -2;
            }
        }
        if (keep_events) {
            current_event.coinc_id = no_coinc_events;
            no_coinc_events++;
        } else {
            current_event.coinc_id = -2;
            this->inc_no_dropped(no_events);
        }
    }
    return (cur_iter);
}
