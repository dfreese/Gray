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
#include <Physics/Interaction.h>

class Process {
public:
    using EventT = Interaction;
    using EventIter = std::vector<EventT>::iterator;
    using TimeT = decltype(EventT::time);
    using DetIdT = decltype(EventT::det_id);

    Process() = default;
    virtual ~Process() = default;

    EventIter process_events(EventIter begin, EventIter end);
    void stop(EventIter begin, EventIter end);
    void reset();
    long no_events() const;
    long no_dropped() const;
    long no_kept() const;

protected:
    void inc_no_dropped();
    void inc_no_dropped(long val);
    static TimeT get_time(const EventT& event);
    static TimeT time_diff(const EventT& lhs, const EventT& rhs);
    static DetIdT get_det_id(const EventT& event);

private:
    virtual EventIter _process_events(EventIter begin, EventIter end) = 0;
    virtual void _stop(EventIter begin, EventIter end) = 0;
    virtual void _reset() = 0;


    /*!
     * Number of events seen by the map
     */
    long count_events = 0;

    /*!
     * Number of events merged into others by the map.
     */
    long count_dropped = 0;

    /*!
     * Number of events kept.
     */
    long count_kept = 0;
};
#endif /* processor_h */
