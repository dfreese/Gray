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
class TimeDiffType = std::function<TimeT(const EventT&, const EventT&)>
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
                 TimeT delay_win_offset = TimeT()) :
        coinc_window(coinc_win),
        delay_offset(delay_win_offset),
        reject_multiples(reject_multiple_events),
        paralyzable(is_paralyzable),
        use_delayed_window(use_delay),
        deltat_func(dt_func),
        no_coinc_pair_events(0),
        no_coinc_multiples_events(0),
        no_coinc_single_events(0),
        no_delay_pair_events(0),
        no_delay_multiples_events(0),
        no_delay_single_events(0)
    {

    }

    long get_no_coinc_pair_events() const {
        return(no_coinc_pair_events);
    }

    long get_no_coinc_multiples_events() const {
        return(no_coinc_multiples_events);
    }

    long get_no_coinc_singles() const {
        return(no_coinc_single_events);
    }

    long get_no_delay_pairs() const {
        return(no_delay_pair_events);
    }

    long get_no_delay_multiples() const {
        return(no_delay_multiples_events);
    }

    long get_no_delay_singles() const {
        return(no_delay_single_events);
    }


    friend std::ostream & operator << (std::ostream & os,
                                       const CoincProcess & cp)
    {
        os << "events in coinc pair    : " << cp.no_coinc_pair_events << "\n"
           << "events in coinc multiple: " << cp.no_coinc_multiples_events << "\n"
           << "events in coinc single  : " << cp.no_coinc_single_events << "\n";
        if (cp.use_delayed_window) {
           os << "events in delay pair    : " << cp.no_delay_pair_events << "\n"
              << "events in delay multiple: " << cp.no_delay_multiples_events << "\n"
              << "events in delay single  : " << cp.no_delay_single_events << "\n";
        }
        return(os);
    }
    

private:

    void _add_event(const EventT & event) {
        buffer.push_back({event, {0, 0}});
        update_buffer_status(false);
    }

    /*!
     *
     */
    void _reset() {
        buffer.clear();
        no_coinc_pair_events = 0;
        no_coinc_multiples_events = 0;
        no_coinc_single_events = 0;
        no_delay_pair_events = 0;
        no_delay_multiples_events = 0;
        no_delay_single_events = 0;
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
            if (ref_event.second.no_coinc) {
                continue;
            }
            TimeT coinc_window_end = coinc_window;
            std::vector<size_t> in_window;
            bool window_timed_out = false;
            for (size_t jj = ii; jj < buffer.size(); jj++) {
                EventPair & new_event = buffer[jj];
                TimeT delta_t = deltat_func(new_event.first, ref_event.first);
                if (delta_t < coinc_window_end) {
                    in_window.push_back(jj);
                    if (paralyzable) {
                        coinc_window_end = delta_t + coinc_window;
                    }
                } else {
                    window_timed_out = true;
                }
            }
            if (window_timed_out || stopping) {
                for (auto idx: in_window) {
                    buffer[idx].second.no_coinc = in_window.size();
                }
            }
        }
        if (use_delayed_window) {
            for (size_t ii = 0; ii < buffer.size(); ii++) {
                EventPair & ref_event = buffer[ii];
                if (ref_event.second.no_delay) {
                    continue;
                }
                TimeT delay_window_start = delay_offset;
                TimeT delay_window_end = delay_offset + coinc_window;
                std::vector<size_t> in_window;
                in_window.push_back(ii);
                bool window_timed_out = false;
                for (size_t jj = ii + 1; jj < buffer.size(); jj++) {
                    EventPair & new_event = buffer[jj];
                    TimeT delta_t = deltat_func(new_event.first,
                                                ref_event.first);
                    bool inside_delay_end = (delta_t < delay_window_end);
                    bool inside_delay_start = (delta_t < delay_window_start);
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
                if (window_timed_out || stopping) {
                    for (auto idx: in_window) {
                        buffer[idx].second.no_delay = in_window.size();
                    }
                }
            }
        }

        auto ready_iter = buffer.cbegin();
        if (use_delayed_window) {
            auto ready_func = [](const EventPair & p){
                return(!p.second.no_coinc || !p.second.no_delay);
            };
            ready_iter = std::find_if(buffer.cbegin(), buffer.cend(),
                                      ready_func);
        } else {
            auto ready_func = [](const EventPair & p){
                return(!p.second.no_coinc);
            };
            ready_iter = std::find_if(buffer.cbegin(), buffer.cend(),
                                      ready_func);
        }

        std::for_each(buffer.cbegin(), ready_iter,
                      [this](const EventPair & p){
                          if (p.second.no_coinc == 2) {
                              no_coinc_pair_events++;
                          } else if (p.second.no_coinc > 2) {
                              no_coinc_multiples_events++;
                          } else {
                              no_coinc_single_events++;
                          }
                          if (p.second.no_delay == 2) {
                              no_delay_pair_events++;
                          } else if (p.second.no_delay > 2) {
                              no_delay_multiples_events++;
                          } else {
                              no_delay_single_events++;
                          }

                          bool keep_event = ((p.second.no_coinc == 2) ||
                                             (p.second.no_delay == 2) ||
                                             (((p.second.no_coinc > 2) ||
                                               (p.second.no_delay > 2)) &&
                                              !this->reject_multiples));
                          if (keep_event) {
                              this->add_ready(p.first);
                          } else {
                              this->inc_no_dropped();
                          }
                      });
        buffer.erase(buffer.cbegin(), ready_iter);
    }

    TimeT coinc_window;
    TimeT delay_offset;
    bool reject_multiples;
    bool paralyzable;
    bool use_delayed_window;

    struct EventInfo {
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

    long no_coinc_pair_events;
    long no_coinc_multiples_events;
    long no_coinc_single_events;
    long no_delay_pair_events;
    long no_delay_multiples_events;
    long no_delay_single_events;

};
#endif // coincprocess_h
