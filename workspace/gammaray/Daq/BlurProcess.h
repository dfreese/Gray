//
//  blurprocess.h
//  graypipeline
//
//  Created by David Freese on 4/6/17.
//
//

#ifndef blurprocess_h
#define blurprocess_h

#include <Daq/Process.h>

class BlurProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using BlurF = std::function<void(EventT&)>;

    BlurProcess(BlurF blurring_func);

private:
    EventIter _process_events(EventIter begin, EventIter end) final;
    void _stop(EventIter begin, EventIter end) final;
    void _reset() final;

    /*!
     * A function that somehow blurs the event
     */
    BlurF blur_func;
};
#endif /* blurprocess_h */
