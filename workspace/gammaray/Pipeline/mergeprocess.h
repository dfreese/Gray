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
     * Adds a new events into detector id map then updates which events are
     * timed out, and will not be merged with any other event.
     */
    std::vector<EventT> _add_events(const std::vector<EventT> & events) {
        if (events.empty()) {
            return(std::vector<EventT>());
        }
        // Merge any events that are still within the window.  Hold onto others
        // where there's nothing from that detector currently, or where the
        // current event is too old.
        std::vector<EventT> local_ready_events;
        for (const auto & event: events) {
            TimeT clear_time = get_time_func(event) - time_window;
            int event_id = find_id(event);
            if (id_event_map.count(event_id) == 0) {
                // Keep the event
                id_event_map[event_id] = event;
            } else if (get_time_func(id_event_map[event_id]) <= clear_time) {
                // Keep the event and push out the old one
                local_ready_events.push_back(id_event_map[event_id]);
                id_event_map[event_id] = event;
            } else {
                // Merge the events together
                merge_func(id_event_map[event_id], event);
                this->inc_no_dropped();
            }
        }

        // Now clear out any events that are too old from the map
        TimeT clear_time = get_time_func(events.back()) - time_window;
        std::unordered_set<int> ids_to_clear;
        for (const auto & id_e_pair: id_event_map) {
            if (get_time_func(id_e_pair.second) <= clear_time) {
                // Keep the event and push out the old one
                local_ready_events.push_back(id_e_pair.second);
                ids_to_clear.insert(id_e_pair.first);
            }
        }

        for (auto det_id: ids_to_clear) {
            id_event_map.erase(det_id);
        }

        // Return the events in time sorted fashion, knowing that events can
        // only have their time go forwards for this type of process, unlike
        // blurring, which can go forward.
        std::sort(local_ready_events.begin(), local_ready_events.end(),
                  [this](const EventT & e0, const EventT & e1){
                      return(this->get_time_func(e0) < this->get_time_func(e1));
                  });
        return(local_ready_events);
    }

    void _reset() {
        id_event_map.clear();
    }

    /*!
     * Simulates the end of the acquisition by saying all events are now fully
     * valid.
     */
    std::vector<EventT> _stop() {
        std::vector<EventT> local_ready_events;
        for (const auto & id_e_pair: id_event_map) {
            local_ready_events.push_back(id_e_pair.second);
        }
        id_event_map.clear();
        // Return the events in time sorted fashion, knowing that events can
        // only have their time go forwards for this type of process, unlike
        // blurring, which can go forward.
        std::sort(local_ready_events.begin(), local_ready_events.end(),
                  [this](const EventT & e0, const EventT & e1){
                      return(this->get_time_func(e0) < this->get_time_func(e1));
                  });
        return(local_ready_events);
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

    std::unordered_map<int, EventT> id_event_map;
};
#endif /* mergemap_h */
