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
    /*!
     * Adds a new event into the merge map.  It then updates which events are
     * timed out, and will not be merged with any other event.
     */
    void _add_event(const EventT & event) {
        // First update the map and time out.
        size_t ii = 0;
        TimeT event_time = get_time_func(event);
        for (; ii < event_buf.size(); ii++) {
            EventT & stored_event = event_buf[ii];
            TimeT stored_event_time = get_time_func(stored_event);
            if ((event_time - stored_event_time) < time_window) {
                // We've found an event that is too new, stop looking, and
                // remember to start here next time.
                break;
            } else {
                this->add_ready(event_buf[ii]);
            }
        }
        event_buf.erase(event_buf.begin(), event_buf.begin() + ii);

        // Now check to see if we should merge the event, and if so merge it,
        // otherwise, add into the structure.
        // TODO: replace this with an unordered map, so we don't have to search
        // all of the non-timed out events.
        int event_id = find_id(event);
        bool merged = false;
        for (size_t ii = 0; ii < event_buf.size(); ii++) {
            EventT & stored_event = event_buf[ii];
            int stored_id = find_id(stored_event);
            if (event_id == stored_id) {
                merge_func(stored_event, event);
                this->inc_no_dropped();
                merged = true;
            }
        }
        if (!merged) {
            event_buf.push_back(event);
        }
    }

    void _reset() {
        event_buf.clear();
    }

    /*!
     * Simulates the end of the acquisition by saying all events are now fully
     * valid.
     */
    void _stop() {
        for (auto & event: event_buf) {
            this->add_ready(event);
        }
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
     * A function type that calculates the time difference between two events.
     * First - Second.
     */
    TimeF get_time_func;

    /*!
     * A function that takes in two events and declares if there could be a
     * valid coincidence between the two events. It is expected to return true
     * if the pair could form a valid coincidence.
     */
    ModF merge_func;

    std::vector<EventT> event_buf;
};
#endif /* mergemap_h */
