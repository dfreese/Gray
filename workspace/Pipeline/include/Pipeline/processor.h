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

    /*!
     * Adds a new event into the merge map.  It then updates which events are
     * timed out, and will not be merged with any other event.
     */
    void add_event(const EventT & event) {
        _add_event(event);
        count_events++;
    }

    /*!
     * Walks through a vector and putting it into the processor one at a time.
     */
    void add_events(const std::vector<EventT> & events) {
        for (const auto & event: events) {
            _add_event(event);
        }
        count_events += events.size();
    }

    /*!
     *
     */
    const std::vector<EventT> & get_ready() const {
        return(ready_events);
    }

    /*!
     * Resets the map and clears out all of the events.
     */
    void reset() {
        _reset();
        count_kept = 0;
        count_dropped = 0;
        count_events = 0;
        ready_events.clear();
    }

    /*!
     * Simulates the end of the acquisition by saying no further events will be
     * added.
     */
    void stop() {
        _stop();
    }

    /*!
     * Returns the number of events that have timed out and won't be modified
     * further.
     */
    size_t no_ready() const {
        return(ready_events.size());
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

    /*!
     * Removes all of the ready events.
     */
    void clear() {
        ready_events.clear();
    }

    /*!
     * Clear all of the information currently in the class
     */

protected:
    void inc_no_dropped() {
        count_dropped++;
    }

    void inc_no_dropped(long val) {
        count_dropped += val;
    }

    void add_ready(const EventT & event) {
        count_kept++;
        ready_events.push_back(event);
    }

private:
    virtual void _add_event(const EventT & event) = 0;
    virtual void _reset() = 0;
    virtual void _stop() = 0;


    std::vector<EventT> ready_events;


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
