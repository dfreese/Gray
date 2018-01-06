#include <Daq/FilterProcess.h>

/*!
 *
 */
FilterProcess::FilterProcess(FilterF filter_func) :
    filt_func(filter_func)
{
}

/*!
 *
 */
FilterProcess::EventIter FilterProcess::process(EventIter begin, EventIter end) {
    for (auto iter = begin; iter != end; ++iter) {
        EventT & event = *iter;
        if (!event.dropped) {
            if (this->filt_func(event)) {
                this->inc_no_kept();
            } else {
                this->inc_no_dropped();
            }
        }
    }
    return (end);
};

/*!
 *
 */
void FilterProcess::stop(EventIter begin, EventIter end) {
    process(begin, end);
};

/*!
 *
 */
void FilterProcess::_reset() {
}

