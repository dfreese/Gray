//
//  blurprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef blurprocess_h
#define blurprocess_h

#include <functional>
#include <Daq/Process.h>

class ProcessStats;

class BlurProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using BlurF = std::function<void(EventT&)>;

    BlurProcess(BlurF blurring_func);
    EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const final;

private:
    /*!
     * A function that somehow blurs the event
     */
    BlurF blur_func;
};
#endif /* blurprocess_h */
