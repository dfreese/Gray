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
class TimeF = std::function<TimeT(const EventT&)>,
class TagF = std::function<void(EventT&, long)>
>
class CoincProcess : public Processor<EventT> {
public:
    /*!
     *
     */
    CoincProcess(TimeT coinc_win, TimeF time_func, TagF tag_coinc_func,
                 bool reject_multiple_events = false,
                 bool combinatorial_pair_multiples = false,
                 bool is_paralyzable = true,
                 TimeT win_offset = TimeT()) :
        coinc_window(coinc_win),
        window_offset(win_offset),
        reject_multiples(reject_multiple_events),
        combinatorial_pair_all_multiples(combinatorial_pair_multiples),
        paralyzable(is_paralyzable),
        get_time_func(time_func),
        tag_coinc_with_id_func(tag_coinc_func),
        no_coinc_pair_events(0),
        no_coinc_multiples_events(0),
        no_coinc_single_events(0),
        no_coinc_events(0)
    {

    }

    long get_no_coinc_events() const {
        return(no_coinc_events);
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

    friend std::ostream & operator << (std::ostream & os,
                                       const CoincProcess & cp)
    {
        os << "coinc events            : " << cp.no_coinc_events << "\n"
           << "events in coinc pair    : " << cp.no_coinc_pair_events << "\n"
           << "events in coinc multiple: " << cp.no_coinc_multiples_events << "\n"
           << "events in coinc single  : " << cp.no_coinc_single_events << "\n";
        return(os);
    }
    

private:
    std::vector<EventT> _add_events(const std::vector<EventT> & events) {
        event_buffer.insert(event_buffer.end(), events.begin(), events.end());
        ready_buffer.insert(ready_buffer.end(), events.size(), false);
        return(update_buffer_status(false));
    }

    /*!
     *
     */
    void _reset() {
        event_buffer.clear();
        ready_buffer.clear();
        no_coinc_pair_events = 0;
        no_coinc_multiples_events = 0;
        no_coinc_single_events = 0;
        no_coinc_events = 0;
    }

    /*!
     *
     */
    std::vector<EventT> _stop() {
        return(update_buffer_status(true));
    }

    std::vector<EventT> update_buffer_status(bool stopping) {
        std::vector<EventT> local_ready_events;
        for (size_t ii = 0; ii < event_buffer.size(); ii++) {
            if (ready_buffer[ii]) {
                continue;
            }
            EventT & ref_event = event_buffer[ii];
            TimeT window_start = window_offset;
            TimeT window_end = window_offset + coinc_window;
            std::vector<size_t> in_window;
            in_window.push_back(ii);
            bool window_timed_out = false;
            for (size_t jj = ii + 1; jj < event_buffer.size(); jj++) {
                EventT & new_event = event_buffer[jj];
                TimeT delta_t = (get_time_func(new_event) -
                                 get_time_func(ref_event));
                bool inside_end = (delta_t < window_end);
                bool inside_start = (delta_t < window_start);
                bool within_window = (inside_end && !inside_start);
                if (within_window) {
                    in_window.push_back(jj);
                    if (paralyzable) {
                        window_end = delta_t + coinc_window;
                    }
                }
                if (!inside_end) {
                    window_timed_out = true;
                    break;
                }
            }
            if (window_timed_out || stopping) {
                bool keep_events = false;
                if (in_window.size() == 2) {
                    no_coinc_pair_events += in_window.size();
                    keep_events = true;
                } else if (in_window.size() > 2) {
                    no_coinc_multiples_events += in_window.size();
                    keep_events = !reject_multiples;
                } else {
                    no_coinc_single_events += in_window.size();
                }
                for (size_t idx = 0; idx < in_window.size(); idx++) {
                    ready_buffer[in_window[idx]] = true;
                    if (keep_events) {
                        tag_coinc_with_id_func(event_buffer[in_window[idx]],
                                               no_coinc_events);
                        // combinatorial_pair_all_multiples mimics Gate's
                        // takeAllGoods.  We intentionally keep the coinc event
                        // id the same for right now, that might be something
                        // to change later. combinatorial_pair_all_multiples is
                        // not the default currently.  The default is to write
                        // out all of the events, and let the user sort out what
                        // to do with them.  Sometimes it is easier to be able
                        // make the assumption that coincidences are all pairs
                        // of events.
                        if (combinatorial_pair_all_multiples) {
                            for (size_t jdx = idx+1; jdx < in_window.size(); jdx++) {
                                local_ready_events.push_back(event_buffer[in_window[idx]]);
                                local_ready_events.push_back(event_buffer[in_window[jdx]]);
                            }
                        } else {
                            local_ready_events.push_back(event_buffer[in_window[idx]]);
                        }
                    }
                }
                if (keep_events) {
                    no_coinc_events++;
                } else {
                    this->inc_no_dropped(in_window.size());
                }
            }
        }

        auto ready_iter = std::find_if_not(ready_buffer.begin(),
                                           ready_buffer.end(),
                                           [](bool val){return(val);});
        const size_t no_ready = ready_iter - ready_buffer.begin();

        ready_buffer.erase(ready_buffer.begin(), ready_iter);
        event_buffer.erase(event_buffer.begin(),
                           event_buffer.begin() + no_ready);
        return(local_ready_events);
    }

    TimeT coinc_window;
    TimeT window_offset;
    bool reject_multiples;
    bool combinatorial_pair_all_multiples;
    bool paralyzable;

    std::vector<EventT> event_buffer;
    std::vector<bool> ready_buffer;

    TimeF get_time_func;
    TagF tag_coinc_with_id_func;

    long no_coinc_pair_events;
    long no_coinc_multiples_events;
    long no_coinc_single_events;
    long no_coinc_events;
};
#endif // coincprocess_h
