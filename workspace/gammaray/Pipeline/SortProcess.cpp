#include <Pipeline/SortProcess.h>
#include <algorithm>
#include <iterator>

/*!
 *
 */
SortProcess::SortProcess(TimeT max_time_to_wait) :
    max_wait_time(max_time_to_wait)
{
}

namespace {
/*!
 * Taken from my answer here:
 * https://codereview.stackexchange.com/a/160363/135154
 *
 * Since we know that our sorting is done on mostly sorted data, insertion
 * sort will be faster, and is stable so it's not swapping elements around.
 */
template<class I, class C = std::less<typename std::iterator_traits<I>::value_type>>
void insertion_sort(I begin, I end, C comp = C()) {
    for (auto i = begin; i != end; ++i) {
        auto index = std::upper_bound(begin, i, *i, comp);
        std::rotate(index, i, i + 1);
    }
}
}

/*!
 *
 */
SortProcess::EventIter SortProcess::_process_events(EventIter begin,
                                                    EventIter end)
{
    // The timeout detection in this function requires a non-empty container
    // so if we're given an empty range, bail right away.
    if (begin == end) {
        return(end);
    }

    auto time_cmp = [](const EventT& e0, const EventT& e1) {
        return(get_time(e0) < get_time(e1));
    };
    insertion_sort(begin, end, time_cmp);

    // work back from the end to figure out where we've timed out, so what
    // we will consider sorted.
    auto timed_out = end - 1;
    const TimeT out_time = get_time(*timed_out) - max_wait_time;
    for (; timed_out != begin; --timed_out) {
        if (get_time(*timed_out) <= out_time) {
            break;
        }
    }
    return (timed_out);
};

/*!
 *
 */
void SortProcess::_stop(EventIter begin, EventIter end) {
    _process_events(begin, end);
};

/*!
 *
 */
void SortProcess::_reset() {
}
