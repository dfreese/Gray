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

class FilterProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using FilterF = std::function<bool(EventT&)>;

    FilterProcess(FilterF filter_func);
    EventIter process(EventIter begin, EventIter end) final;
    void stop(EventIter begin, EventIter end) final;

private:
    void _reset() final;

    /*!
     * A function that returns true if the event should be kept.
     */
    FilterF filt_func;
};
#endif /* filterprocess_h */
