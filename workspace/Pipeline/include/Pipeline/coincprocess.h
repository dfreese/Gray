//
//  coincprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef coincprocess_h
#define coincprocess_h

#include <Pipeline/processor.h>

/*!
 *
 */
template <
class EventT,
class TimeT,
class TimeDiffType = std::function<TimeT(const EventT&, const EventT&)>,
class TimeCompT = std::less<TimeT>
>
class CoincProcess : public Processor<EventT> {
public:
    /*!
     *
     */
    CoincProcess(TimeT coinc_win, TimeDiffType dt_func,
                 TimeCompT time_lt_func = TimeCompT()) :
        coinc_window(coinc_win),
        last_event_saved(false),
        last_event_valid(false),
        deltat_func(dt_func),
        time_less_than(time_lt_func)
    {

    }

private:

    void _add_event(const EventT & event) {
        bool within_window = time_less_than(deltat_func(event, last_event),
                                            coinc_window);
        if (within_window && last_event_valid) {
            if (!last_event_saved) {
                this->add_ready(last_event);
                last_event_saved = true;
            }
            this->add_ready(event);
        } else {
            if (!last_event_saved && last_event_valid) {
                this->inc_no_dropped();
            }
            last_event_saved = false;
        }
        last_event_valid = true;
        last_event = event;
    }

    /*!
     *
     */
    void _reset() {
        last_event_saved = false;
        last_event_valid = false;
    }

    /*!
     *
     */
    void _stop() {
        if (!last_event_saved) {
            this->inc_no_dropped();
        }
        last_event_saved = false;
        last_event_valid = false;
    }

    /*!
     *
     */
    void _clear() {
    }

    TimeT coinc_window;
    EventT last_event;
    bool last_event_saved;
    bool last_event_valid;

    /*!
     * A function type that calculates the time difference between two events.
     * First - Second.
     */
    TimeDiffType deltat_func;
    TimeCompT time_less_than;
};
#endif // coincprocess_h
