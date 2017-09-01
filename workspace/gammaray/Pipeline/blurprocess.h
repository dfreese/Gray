//
//  blurprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef blurprocess_h
#define blurprocess_h

#include <Pipeline/processor.h>

/*!
 *
 */
template <class EventT, class BlurF = std::function<void(EventT&)>>
class BlurProcess : public Processor<EventT> {
public:

    /*!
     *
     */
    BlurProcess(BlurF blurring_func) :
        blur_func(blurring_func)
    {
    }

private:
    typedef typename std::vector<EventT>::iterator event_iter;

    event_iter _process_events(event_iter begin, event_iter end) final {
        for (auto iter = begin; iter != end; ++iter) {
            EventT & event = *iter;
            if (!event.dropped) {
                blur_func(event);
            }
        }
        return (end);
    };

    void _stop(event_iter begin, event_iter end) final {
        _process_events(begin, end);
    };

    void _reset() {
    }

    /*!
     * A function that somehow blurs the event
     */
    BlurF blur_func;
};
#endif /* blurprocess_h */
