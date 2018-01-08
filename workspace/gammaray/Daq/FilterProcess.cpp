#include <Daq/FilterProcess.h>
#include <Daq/ProcessStats.h>

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
FilterProcess::EventIter FilterProcess::process(
        EventIter begin, EventIter end,
        ProcessStats& stats) const
{
    for (auto iter = begin; iter != end; ++iter) {
        EventT & event = *iter;
        if (!event.dropped) {
            if (this->filt_func(event)) {
                stats.no_kept++;
            } else {
                stats.no_dropped++;
            }
        }
    }
    return (end);
};

