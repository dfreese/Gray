//
//  coincprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef coincprocess_h
#define coincprocess_h

#include <Daq/Process.h>

class CoincProcess : public Process {
public:

    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;

    CoincProcess(TimeT coinc_win, bool reject_multiple_events,
                 bool is_paralyzable, TimeT win_offset);
    EventIter process(EventIter begin, EventIter end) final;
    void stop(EventIter begin, EventIter end) final;

    long get_no_coinc_events() const;
    long get_no_coinc_pair_events() const;
    long get_no_coinc_multiples_events() const;
    long get_no_coinc_singles() const;

private:
    void _reset() final;
    std::string print_info() const final;
    EventIter process_events_optional_stop(EventIter begin, EventIter end,
                                           bool stopping);

    TimeT coinc_window;
    TimeT window_offset;
    bool reject_multiples;
    bool paralyzable;

    long no_coinc_pair_events;
    long no_coinc_multiples_events;
    long no_coinc_single_events;
    long no_coinc_events;
};
#endif // coincprocess_h
