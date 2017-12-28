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
private:
    EventIter _process_events(EventIter begin, EventIter end);
    void _stop(EventIter begin, EventIter end) final;
    void _reset() final;

    /*!
     * A function that returns true if the event should be kept.
     */
    FilterF filt_func;
};
#endif /* filterprocess_h */
