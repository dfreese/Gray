#include <Pipeline/BlurProcess.h>

/*!
 *
 */
BlurProcess::BlurProcess(BlurF blurring_func) :
    blur_func(blurring_func)
{
}

/*!
 *
 */
BlurProcess::EventIter BlurProcess::_process_events(EventIter begin,
                                                    EventIter end)
{
    for (auto iter = begin; iter != end; ++iter) {
        EventT & event = *iter;
        if (!event.dropped) {
            blur_func(event);
        }
    }
    return (end);
}

/*!
 *
 */
void BlurProcess::_stop(EventIter begin, EventIter end) {
    _process_events(begin, end);
}

/*!
 *
 */
void BlurProcess::_reset() {
}
