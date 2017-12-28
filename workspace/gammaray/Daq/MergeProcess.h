//
//  mergeprocess.h
//  graypipeline
//
//  Created by David Freese on 2/1/17.
//
//

#ifndef mergeprocess_h
#define mergeprocess_h

#include <Daq/Process.h>

class MergeProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;
    using DetIdT = Process::DetIdT;
    using MergeF = std::function<void(EventT&, EventT&)>;

    MergeProcess(const std::vector<DetIdT> & lookup, TimeT t_window,
                 MergeF merge_fc);

private:
    EventIter _process_events(EventIter begin, EventIter end) final;
    void _stop(EventIter begin, EventIter end) final;
    void _reset() final;
    DetIdT mapped_id(const EventT& event) const;

    /*!
     * A lookup table for the component id that is associated with each
     * detector id.
     */
    const std::vector<DetIdT> id_lookup;

    TimeT time_window;

    /*!
     * A function that takes in two events and declares if there could be a
     * valid coincidence between the two events. It is expected to return true
     * if the pair could form a valid coincidence.
     */
    MergeF merge_func;
};
#endif /* mergemap_h */
