//
//  sortprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef sortprocess_h
#define sortprocess_h

#include <Pipeline/processor.h>
#include <queue>
#include <vector>

/*!
 *
 */
template <
class EventT,
class TimeT,
class TimeF = std::function<TimeT(const EventT&)>
>
class SortProcess : public Processor<EventT> {
public:
    typedef std::function<bool(const EventT&, const EventT&)> EventTimeGreaterThanT;
    /*!
     *
     */
    SortProcess(TimeT max_time_to_wait, TimeF time_func) :
        max_wait_time(max_time_to_wait),
        get_time_func(time_func),
        event_min_heap([time_func](const EventT& e0, const EventT& e1) {
            return(time_func(e0) > time_func(e1));})
    {

    }

private:
    void _add_events(const std::vector<EventT> & events) {
        for (const auto & event : events) {
            event_min_heap.push(event);
            TimeT event_time = get_time_func(event);
            max_time = std::max(event_time, max_time);
        }

        std::vector<EventT> local_events_ready;
        while(!event_min_heap.empty()) {
            const EventT & stored_event = event_min_heap.top();
            TimeT time = get_time_func(stored_event);
            if ((max_time - time) < max_wait_time) {
                // We've found an event that is too new, stop looking, and
                // remember to start here next time.
                break;
            } else {
                local_events_ready.push_back(stored_event);
                event_min_heap.pop();
            }
        }
        this->add_ready(local_events_ready);
    }

    void _reset() {
        while(!event_min_heap.empty()) {
            event_min_heap.pop();
        }
    }

    /*!
     *
     */
    void _stop() {
        std::vector<EventT> local_events_ready;
        while(!event_min_heap.empty()) {
            local_events_ready.push_back(event_min_heap.top());
            event_min_heap.pop();
        }
        // TODO: check that initialization of time will be okay
        max_time = TimeT();
        this->add_ready(local_events_ready);
    }

    /*!
     *
     */
    void _clear() {
    }

    TimeT max_wait_time;
    TimeT max_time;

    /*!
     * A function type that calculates the time difference between two events.
     * First - Second.
     */
    TimeF get_time_func;
    std::priority_queue<EventT, std::vector<EventT>, EventTimeGreaterThanT> event_min_heap;
};
#endif // sortprocess_h
