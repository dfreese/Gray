//
//  sortprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef sortprocess_h
#define sortprocess_h

#include <Pipeline/Process.h>

class SortProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;

    SortProcess(TimeT max_time_to_wait);

private:
    EventIter _process_events(EventIter begin, EventIter end) final;
    void _stop(EventIter begin, EventIter end) final;
    void _reset() final;

    TimeT max_wait_time;
};
#endif // sortprocess_h
