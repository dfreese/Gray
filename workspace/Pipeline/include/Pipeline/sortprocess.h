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
class TimeDiffType = std::function<TimeT(const EventT&, const EventT&)>
>
class SortProcess : public Processor<EventT> {
public:
    typedef std::function<bool(const EventT&, const EventT&)> EventTimeGreaterThanT;
    /*!
     *
     */
    SortProcess(TimeT max_time_to_wait, TimeDiffType dt_func) :
        max_wait_time(max_time_to_wait),
        deltat_func(dt_func),
        event_min_heap([dt_func](const EventT& e0, const EventT& e1) {
            return(0 < dt_func(e0, e1));})
    {

    }

private:
    void _add_event(const EventT & event) {

        event_min_heap.push(event);
        max_time_event = (0 < deltat_func(event, max_time_event)) ?
                event : max_time_event;

        while(!event_min_heap.empty()) {
            const EventT & stored_event = event_min_heap.top();
            TimeT delta_t = deltat_func(max_time_event, stored_event);
            if (delta_t < max_wait_time) {
                // We've found an event that is too new, stop looking, and
                // remember to start here next time.
                break;
            } else {
                this->add_ready(stored_event);
                event_min_heap.pop();
            }
        }
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
        while(!event_min_heap.empty()) {
            this->add_ready(event_min_heap.top());
            event_min_heap.pop();
        }
        // TODO: check that initialization of time will be okay
        max_time_event = EventT();
    }

    /*!
     *
     */
    void _clear() {
    }

    TimeT max_wait_time;
    EventT max_time_event;

    /*!
     * A function type that calculates the time difference between two events.
     * First - Second.
     */
    TimeDiffType deltat_func;
    std::priority_queue<EventT, std::vector<EventT>, EventTimeGreaterThanT> event_min_heap;
};
#endif // sortprocess_h
