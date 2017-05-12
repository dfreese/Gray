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
    std::vector<EventT> _add_events(const std::vector<EventT> & events) {
        std::vector<EventT> new_events(events);
        std::for_each(new_events.begin(), new_events.end(), blur_func);
        return(new_events);
    }

    void _reset() {
    }

    /*!
     *
     */
    std::vector<EventT> _stop() {
        return(std::vector<EventT>());
    }

    /*!
     * A function that somehow blurs the event
     */
    BlurF blur_func;
};
#endif /* blurprocess_h */
