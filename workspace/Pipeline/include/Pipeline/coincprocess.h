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
        buffer.push_back({event, {false, false, false, false}});
        update_buffer_status(false);
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
        update_buffer_status(true);
        buffer.clear();
    }

    void update_buffer_status(bool stopping) {
        for (size_t ii = 0; ii < buffer.size(); ii++) {
            EventPair & ref_event = buffer[ii];
            if (ref_event.second.in_coinc) {
                continue;
            }
            TimeT coinc_window_end = coinc_window;
            std::vector<size_t> in_window;
            bool window_timed_out = false;
            for (size_t jj = ii; jj < buffer.size(); jj++) {
                EventPair & new_event = buffer[jj];
                TimeT delta_t = deltat_func(new_event.first, ref_event.first);
                bool within_coinc_window = time_less_than(delta_t,
                                                          coinc_window_end);
                if (within_coinc_window) {
                    in_window.push_back(jj);
                    if (paralyzable) {
                        coinc_window_end = delta_t + coinc_window;
                    }
                } else {
                    window_timed_out = true;
                }
            }
            bool keep_coinc = ((in_window.size() == 2) ||
                               ((in_window.size() > 2) && !reject_multiples));
            if (window_timed_out || stopping) {
                for (auto idx: in_window) {
                    buffer[idx].second.write_coinc = keep_coinc;
                    buffer[idx].second.in_coinc = true;
                }
            }
        }
        for (size_t ii = 0; ii < buffer.size(); ii++) {
            EventPair & ref_event = buffer[ii];
            if (ref_event.second.in_delay) {
                continue;
            }
            TimeT delay_window_start = delay_offset;
            TimeT delay_window_end = delay_offset + coinc_window;
            std::vector<size_t> in_window;
            in_window.push_back(ii);
            bool window_timed_out = false;
            for (size_t jj = ii + 1; jj < buffer.size(); jj++) {
                EventPair & new_event = buffer[jj];
                TimeT delta_t = deltat_func(new_event.first, ref_event.first);
                bool inside_delay_end = time_less_than(delta_t,
                                                       delay_window_end);
                bool inside_delay_start = time_less_than(delta_t,
                                                         delay_window_start);
                bool within_delay_window = (inside_delay_end &&
                                            !inside_delay_start);
                if (within_delay_window) {
                    in_window.push_back(jj);
                    if (paralyzable) {
                        delay_window_end = delta_t + coinc_window;
                    }
                }
                if (!inside_delay_end) {
                    window_timed_out = true;
                }
            }
            bool keep_delay = ((in_window.size() == 2) ||
                               ((in_window.size() > 2) && !reject_multiples));
            if (window_timed_out || stopping) {
                for (auto idx: in_window) {
                    buffer[idx].second.write_delay = keep_delay;
                    buffer[idx].second.in_delay = true;
                }
            }
        }

        auto find_ready = std::find_if(buffer.cbegin(), buffer.cend(),
                                       [](const EventPair & p){
                                           return(!p.second.in_coinc ||
                                                  !p.second.in_delay);
                                       });

        std::for_each(buffer.cbegin(), find_ready,
                      [this](const EventPair & p){
                          if (p.second.write_coinc || p.second.write_delay) {
                              this->add_ready(p.first);
                          } else {
                              this->inc_no_dropped();
                          }
                      });
        buffer.erase(buffer.cbegin(), find_ready);
    }

    TimeT coinc_window;
    TimeT delay_offset;
    bool reject_multiples;
    bool paralyzable;
    bool use_delayed_window;


    int no_delay_events;

    struct EventInfo {
        bool write_coinc;
        bool write_delay;
        bool in_coinc;
        bool in_delay;
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
