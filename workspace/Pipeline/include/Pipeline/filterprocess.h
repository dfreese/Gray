//
//  filterprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef filterprocess_h
#define filterprocess_h

#include <fstream>
#include <vector>
#include <Pipeline/processor.h>

/*!
 * Creates a base class for events rolling merge of a user specified type.
 */
template <class EventT, class FilterF = std::function<bool(const EventT&)>>
class FilterProcess : public Processor<EventT> {
public:

    /*!
     *
     */
    FilterProcess(FilterF filter_func) :
        filt_func(filter_func)
    {
    }

private:
    /*!
     * Adds a new event into the merge map.  It then updates which events are
     * timed out, and will not be merged with any other event.
     */
    void _add_events(const std::vector<EventT> & events) {
        std::vector<EventT> local_ready_events(std::count_if(events.cbegin(),
                                                             events.cend(),
                                                             filt_func));
        std::copy_if(events.cbegin(), events.cend(),
                     local_ready_events.begin(), filt_func);
        this->inc_no_dropped(local_ready_events.size() - events.size());
        this->add_ready(local_ready_events);
    }

    void _reset() {
    }

    /*!
     * Simulates the end of the acquisition by saying all events are now fully
     * valid.
     */
    void _stop() {
    }

    /*!
     * A function that returns true if the event should be kept.
     */
    FilterF filt_func;
};
#endif /* filterprocess_h */
