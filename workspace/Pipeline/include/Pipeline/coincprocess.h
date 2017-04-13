//
//  coincprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef coincprocess_h
#define coincprocess_h

#include <vector>
#include <algorithm>
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
                 bool reject_multiple_events = false,
                 bool is_paralyzable = true,
                 bool use_delay = false,
                 TimeT delay_win_offset = TimeT(),
                 TimeCompT time_lt_func = TimeCompT()) :
        coinc_window(coinc_win),
        delay_offset(delay_win_offset),
        reject_multiples(reject_multiple_events),
        paralyzable(is_paralyzable),
        use_delayed_window(use_delay),
        deltat_func(dt_func),
        time_less_than(time_lt_func)
    {

    }

private:

    void _add_event(const EventT & event) {
        if (buffer.empty()) {
            coinc_window_end = coinc_window;
            delay_window_start = delay_offset;
            delay_window_end = delay_offset + coinc_window;
            // set acted_on for this event as it created the window.  The same
            // will need to be done when a window is reopened without an empty
            // buffer.
            buffer.push_back({event, {false, false, 0, 0}});
            return;
        }
        // Add the event to the buffer
        buffer.push_back({event, {false, false, 0, 0}});

        EventPair & ref_event_pair = buffer.front();
        EventPair & new_event_pair = buffer.back();

        TimeT delta_t = deltat_func(new_event_pair.first,
                                    ref_event_pair.first);

        bool within_coinc_window = time_less_than(delta_t, coinc_window_end);
        if (within_coinc_window) {
            ref_event_pair.second.no_coinc++;
            new_event_pair.second.no_coinc++;
            if (paralyzable) {
                coinc_window_end += delta_t;
            }
            if (reject_multiples && (ref_event_pair.second.no_coinc > 1)) {
                ref_event_pair.second.write_coinc = false;
                new_event_pair.second.write_coinc = false;
            } else {
                ref_event_pair.second.write_coinc = true;
                new_event_pair.second.write_coinc = true;
            }
            // If we're not going to do any processing on a delayed window,
            // then we can return right now, because we haven't seen the end of
            // the coincidence window.
            if (!use_delayed_window) {
                return;
            }
        }

        bool inside_delay_end = time_less_than(delta_t, delay_window_end);
        bool inside_delay_start = time_less_than(delta_t, delay_window_start);
        bool within_delay_window = inside_delay_end && !inside_delay_start;
        if (use_delayed_window && inside_delay_end) {
            if (within_delay_window) {
                ref_event_pair.second.no_delay++;
                new_event_pair.second.no_delay++;
                if (paralyzable) {
                    delay_window_end += delta_t;
                }
                if (reject_multiples && (ref_event_pair.second.no_delay > 1)) {
                    ref_event_pair.second.write_delay = false;
                    new_event_pair.second.write_delay = false;
                } else {
                    ref_event_pair.second.write_delay = true;
                    new_event_pair.second.write_delay = true;
                }
            }
            // Nothing else to do, since the window hasn't timed out yet.
            return;
        }

        // Find all of the events that have been acted on by the coincidence
        // window and thus will be removed one way or another.  Assume the first
        // event has been acted on.
        EventPairVecCIter save_iter = std::find_if(buffer.begin() + 1,
                buffer.end(), [](const EventPair & p){
                    return(p.second.no_coinc == 0);
                });

        std::for_each(buffer.cbegin(), save_iter,
                      [this](const EventPair & p){
                          if (p.second.write_coinc || p.second.write_delay) {
                              this->add_ready(p.first);
                          } else {
                              this->inc_no_dropped();
                          }
                      });
        buffer.erase(buffer.cbegin(), save_iter);

        // Reset window to first event if necessary (we assume the first event
        // is at time = zero).
        if (!buffer.empty()) {
            coinc_window_end = coinc_window;
            delay_window_start = delay_offset;
            delay_window_end = delay_offset + coinc_window;
        }
    }

    /*!
     *
     */
    void _reset() {
        buffer.clear();
    }

    /*!
     *
     */
    void _stop() {
        std::for_each(buffer.cbegin(), buffer.cend(),
                      [this](const EventPair & p){
                          if (p.second.write_coinc || p.second.write_delay) {
                              this->add_ready(p.first);
                          } else {
                              this->inc_no_dropped();
                          }
                      });
        buffer.clear();
    }

    /*!
     *
     */
    void _clear() {
    }

    TimeT coinc_window;
    TimeT delay_offset;
    bool reject_multiples;
    bool paralyzable;
    bool use_delayed_window;

    TimeT coinc_window_end;
    TimeT delay_window_start;
    TimeT delay_window_end;

    struct EventInfo {
        bool write_coinc;
        bool write_delay;
        int no_coinc;
        int no_delay;
    };
    typedef std::pair<EventT, EventInfo> EventPair;
    typedef std::vector<EventPair> EventPairVec;
    typedef typename EventPairVec::iterator EventPairVecIter;
    typedef typename EventPairVec::const_iterator EventPairVecCIter;

    EventPairVec buffer;

    /*!
     * A function type that calculates the time difference between two events.
     * First - Second.
     */
    TimeDiffType deltat_func;
    TimeCompT time_less_than;
};
#endif // coincprocess_h
