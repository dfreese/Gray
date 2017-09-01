//
//  mergeprocess.h
//  graypipeline
//
//  Created by David Freese on 2/1/17.
//
//

#ifndef mergeprocess_h
#define mergeprocess_h

#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <Pipeline/processor.h>

/*!
 * Creates a base class for events rolling merge of a user specified type.
 */
template <
class EventT,
class TimeT,
class TimeF = std::function<TimeT(const EventT&)>,
class InfoF = std::function<int(const EventT&)>,
class ModF = std::function<void(EventT&, const EventT&)>
>

class MergeProcess : public Processor<EventT> {
public:

    /*!
     *
     */
    MergeProcess(const std::vector<int> & lookup, TimeT t_window,
                 TimeF time_func, InfoF id_func, ModF merge_fc) :
        id_lookup(lookup),
        no_detectors(lookup.size()),
        time_window(t_window),
        get_id_func(id_func),
        get_time_func(time_func),
        merge_func(merge_fc)
    {
    }

private:
    typedef typename std::vector<EventT>::iterator event_iter;

    event_iter _process_events(event_iter begin, event_iter end) final {
        auto cur_iter = begin;
        for (; cur_iter != end; cur_iter++) {
            EventT & cur_event = *cur_iter;
            if (cur_event.dropped) {
                continue;
            }
            const int current_event_id = find_id(cur_event);
            // Check to see where this event times out
            const TimeT window = get_time_func(cur_event) + time_window;
            auto next_iter = cur_iter + 1;
            for (; next_iter != end; next_iter++) {
                EventT & next_event = *next_iter;
                if (next_event.dropped) {
                    continue;
                }
                if (get_time_func(next_event) >= window) {
                    break;
                }
                if (current_event_id == find_id(next_event)) {
                    merge_func(cur_event, next_event);
                    next_event.dropped = true;
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
     * First - Second.
     */
    TimeF get_time_func;

    /*!
     * A function that takes in two events and declares if there could be a
     * valid coincidence between the two events. It is expected to return true
     * if the pair could form a valid coincidence.
     */
    ModF merge_func;
};
#endif /* mergemap_h */
