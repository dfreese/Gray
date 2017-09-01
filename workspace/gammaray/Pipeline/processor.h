//
//  processor.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef processor_h
#define processor_h

#include <vector>
#include <algorithm>

/*!
 * Creates a base class processing a stream of events of a user specified type.
 */
template <class EventT>
class Processor {
public:
    /*!
     *
     */
    Processor() :
        count_events(0),
        count_dropped(0),
        count_kept(0)
    {}

    virtual ~Processor() {};

    typedef typename std::vector<EventT>::iterator event_iter;

    event_iter process_events(event_iter begin, event_iter end) {
        const long start_count_dropped = count_dropped;
        auto ready_events = _process_events(begin, end);

        auto kept_func = [](const EventT & event) {return (!event.dropped);};
        const long kept = std::count_if(begin, ready_events, kept_func);
        count_kept += kept;
        count_events += kept + (count_dropped - start_count_dropped);
        return(ready_events);
    }

    void stop(event_iter begin, event_iter end) {
        const long start_count_dropped = count_dropped;

        _stop(begin, end);

        auto kept_func = [](const EventT & event) {return (!event.dropped);};
        const long kept = std::count_if(begin, end, kept_func);
        count_kept += kept;
        count_events += kept + (count_dropped - start_count_dropped);
    }

    /*!
     * Resets the map and clears out all of the events.
     */
    void reset() {
        _reset();
        count_kept = 0;
        count_dropped = 0;
        count_events = 0;
    }

    /*!
     * Number of events processed by the class
     */
    long no_events() const {
        return(count_events);
    }

    /*!
     * Number of events dropped or merged by the class
     */
    long no_dropped() const {
        return(count_dropped);
    }

    /*!
     * Number of events output by the class.
     */
    long no_kept() const {
        return(count_kept);
    }

protected:
    void inc_no_dropped() {
        count_dropped++;
    }

    void inc_no_dropped(long val) {
        count_dropped += val;
    }

private:
    virtual event_iter _process_events(event_iter begin, event_iter end) = 0;
    virtual void _stop(event_iter begin, event_iter end) = 0;
    virtual void _reset() = 0;


    /*!
     * Number of events seen by the map
     */
    long count_events;

    /*!
     * Number of events merged into others by the map.
     */
    long count_dropped;

    /*!
     * Number of events kept.
     */
    long count_kept;
};
#endif /* processor_h */
