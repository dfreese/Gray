//
//  processor.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef processor_h
#define processor_h

#include <algorithm>
#include <string>
#include <vector>
#include <Physics/Interaction.h>

class Process {
public:
    using EventT = Interaction;
    using ContainerT = std::vector<EventT>;
    using EventIter = ContainerT::iterator;
    using TimeT = decltype(EventT::time);
    using DetIdT = decltype(EventT::det_id);

    Process() = default;
    virtual ~Process() = default;

    virtual EventIter process(EventIter begin, EventIter end) = 0;
    virtual void stop(EventIter begin, EventIter end) = 0;
    void reset();
    long no_events() const;
    long no_dropped() const;
    long no_kept() const;
    friend std::ostream & operator << (std::ostream & os, const Process & cp);

protected:
    void inc_no_kept();
    void inc_no_kept(long val);
    void inc_no_dropped();
    void inc_no_dropped(long val);

private:
    virtual void _reset() = 0;
    virtual std::string print_info() const;

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
