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

    typedef typename std::vector<EventT>::iterator event_iter;

    event_iter _process_events(event_iter begin, event_iter end) final {
        auto current_event = begin;
        for (; current_event != end; current_event++) {
            if ((*current_event).dropped) {
                continue;
            }
            const int current_event_id = find_id(*current_event);
            // Check to see where this event times out
            TimeT window = get_time_func(*current_event) + time_window;
            auto next_event = current_event + 1;
            for (; next_event != end; next_event++) {
                if ((*next_event).dropped) {
                    continue;
                }

                const TimeT next_time = get_time_func(*next_event);
                if (next_time >= window) {
                    break;
                }
                if (current_event_id == find_id(*next_event)) {
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

    void _stop(event_iter begin, event_iter end) final {
        _process_events(begin, end);
    };

    void _reset() final {
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
     * A function type that returns the time of an event.
     */
    TimeF get_time_func;

    bool is_paralyzable;
};
#endif // deadtimeprocess_h
