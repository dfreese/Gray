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

class ProcessStats;

class CoincProcess : public Process {
public:

    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;

    CoincProcess(TimeT coinc_win, bool reject_multiple_events,
                 bool is_paralyzable, TimeT win_offset);
    EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const final;
    void stop(EventIter begin, EventIter end, ProcessStats& stats) const final;

private:
    EventIter process_events_optional_stop(
            EventIter begin, EventIter end,
            ProcessStats& stats, bool stopping) const;

    TimeT coinc_window;
    TimeT window_offset;
    bool reject_multiples;
    bool paralyzable;
};
#endif // coincprocess_h
