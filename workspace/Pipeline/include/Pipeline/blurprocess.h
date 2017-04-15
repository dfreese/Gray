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
    void _add_event(const EventT & event) {
        // TODO: fix this so there's not nearly so much copying going on. Dumb.
        EventT event_copy = event;
        blur_func(event_copy);
        this->add_ready(event_copy);
    }

    void _reset() {
    }

    /*!
     *
     */
    void _stop() {
    }

    /*!
     * A function that somehow blurs the event
     */
    BlurF blur_func;
};
#endif /* blurprocess_h */
