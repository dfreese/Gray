//
//  deadtimeprocess.h
//  graypipeline
//
//  Created by David Freese on 4/17/17.
//
//

#ifndef deadtimeprocess_h
#define deadtimeprocess_h

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <Pipeline/processor.h>

/*!
 * Creates a base class for events rolling merge of a user specified type.
 */
template <
class EventT,
class TimeT,
class TimeF = std::function<TimeT(const EventT&)>,
class InfoF = std::function<int(const EventT&)>
>
class DeadtimeProcess : public Processor<EventT> {
public:

    /*!
     *
     */
    DeadtimeProcess(const std::vector<int> & lookup, TimeT deadtime,
                    TimeF time_func, InfoF det_id_fun, bool paralyzable) :
        id_lookup(lookup),
        no_detectors(lookup.size()),
        time_window(deadtime),
        get_id_func(det_id_fun),
        get_time_func(time_func),
        is_paralyzable(paralyzable)
    {
    }

private:
    /*!
     *
     */
    std::vector<EventT> _add_events(const std::vector<EventT> & events) {
        if (events.empty()) {
            return(std::vector<EventT>());
        }
        for (const auto event: events) {
            TimeT event_time = get_time_func(event);
            int event_id = find_id(event);
            if (timeouts.count(event_id) == 0) {
                // Keep the event
                event_buf.push_back(event);
                timeouts[event_id] = event_time + time_window;
            } else if (timeouts[event_id] <= event_time) {
                // Keep the event
                event_buf.push_back(event);
                timeouts[event_id] = event_time + time_window;
            } else if (is_paralyzable) {
                // Drop and extend the window
                timeouts[event_id] = event_time + time_window;
                this->inc_no_dropped();
            } else {
                // Just drop
                this->inc_no_dropped();
            }
        }

        // Remove any chunk of events that are timed out at the beginning.
        TimeT event_time = get_time_func(events.back());
        auto ready_iter = std::find_if(event_buf.begin(), event_buf.end(),
                                       [this, event_time](const EventT & e) {
                                           int det_id = this->find_id(e);
                                           if (event_time < timeouts[det_id]) {
                                               return(true);
                                           } else {
                                               return(false);
                                           }
                                       });
        std::vector<EventT> local_ready_events(event_buf.begin(), ready_iter);
        event_buf.erase(event_buf.begin(), ready_iter);
        return(local_ready_events);
    }

    void _reset() {
        event_buf.clear();
    }

    /*!
     * Simulates the end of the acquisition by saying all events are now fully
     * valid.
     */
    std::vector<EventT> _stop() {
        std::vector<EventT> tmp_empty;
        tmp_empty.swap(event_buf);
        return(tmp_empty);
    }

    /*!
     * A lookup table for the component id that is associated with each
     * detector id.
     */
    const std::vector<int> id_lookup;

    /*!
     * The number of detectors.  id_func should work from 0 to no_detectors - 1.
     */
    const int no_detectors;

    int find_id(const EventT& event) {
        return(id_lookup[get_id_func(event)]);
    }

    TimeT time_window;

    /*!
     * Returns the detector id for the event.  This is then mapped to a.
     */
    InfoF get_id_func;

    /*!
     *
     */
    TimeF get_time_func;

    bool is_paralyzable;

    std::vector<EventT> event_buf;
    std::unordered_map<int, TimeT> timeouts;
};
#endif // deadtimeprocess_h
