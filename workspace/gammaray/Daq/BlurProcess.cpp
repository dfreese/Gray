#include <Daq/BlurProcess.h>

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
BlurProcess::EventIter BlurProcess::process(EventIter begin, EventIter end) {
    for (auto iter = begin; iter != end; ++iter) {
        EventT & event = *iter;
        if (!event.dropped) {
            this->inc_no_kept();
            blur_func(event);
        }
    }
    return (end);
}

/*!
 *
 */
void BlurProcess::stop(EventIter begin, EventIter end) {
    process(begin, end);
}

/*!
 *
 */
void BlurProcess::_reset() {
}
