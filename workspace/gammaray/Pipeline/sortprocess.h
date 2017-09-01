//
//  sortprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef sortprocess_h
#define sortprocess_h

#include <Pipeline/processor.h>
#include <vector>
#include <algorithm>

/*!
 *
 */
template <
class EventT,
class TimeT,
class TimeF = std::function<TimeT(const EventT&)>
>
class SortProcess : public Processor<EventT> {
public:
    /*!
     *
     */
    SortProcess(TimeT max_time_to_wait, TimeF time_func) :
        max_wait_time(max_time_to_wait),
        get_time_func(time_func)
    {

    }

private:
    typedef typename std::vector<EventT>::iterator event_iter;


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

    event_iter _process_events(event_iter begin, event_iter end) final {
        // The timeout detection in this function requires a non-empty container
        // so if we're given an empty range, bail right away.
        if (begin == end) {
            return(end);
        }

        auto time_func = this->get_time_func;
        auto time_cmp = [time_func](const EventT& e0, const EventT& e1) {
            return(time_func(e0) < time_func(e1));
        };
        insertion_sort(begin, end, time_cmp);

        // work back from the end to figure out where we've timed out, so what
        // we will consider sorted.
        auto timed_out = end - 1;
        const TimeT out_time = get_time_func(*timed_out) - max_wait_time;
        for (; timed_out != begin; --timed_out) {
            if (get_time_func(*timed_out) <= out_time) {
                break;
            }
        }
        return (timed_out);
    };

    void _stop(event_iter begin, event_iter end) final {
        _process_events(begin, end);
    };

    void _reset() {
    }

    TimeT max_wait_time;

    /*!
     * A function type that returns the time of an event.
     */
    TimeF get_time_func;
};
#endif // sortprocess_h
