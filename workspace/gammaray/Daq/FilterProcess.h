//
//  filterprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef filterprocess_h
#define filterprocess_h

#include <Daq/Process.h>

class ProcessStats;

class FilterProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using FilterF = std::function<bool(EventT&)>;

    FilterProcess(FilterF filter_func);
    EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const final;

private:
    /*!
     * A function that returns true if the event should be kept.
     */
    FilterF filt_func;
};
#endif /* filterprocess_h */
