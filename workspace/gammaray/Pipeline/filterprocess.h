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
template <class EventT, class FilterF = std::function<bool(EventT&)>>
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
    typedef typename std::vector<EventT>::iterator event_iter;

    event_iter _process_events(event_iter begin, event_iter end) final {
        for (auto iter = begin; iter != end; ++iter) {
            EventT & event = *iter;
            if (!event.dropped) {
                if (!this->filt_func(event)) {
                    this->inc_no_dropped();
                }
            }
        }
        return (end);
    };

    void _stop(event_iter begin, event_iter end) final {
        _process_events(begin, end);
    };

    void _reset() final {
    }

    /*!
     * A function that returns true if the event should be kept.
     */
    FilterF filt_func;
};
#endif /* filterprocess_h */
