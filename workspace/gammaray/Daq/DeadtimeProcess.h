//
//  deadtimeprocess.h
//  graypipeline
//
//  Created by David Freese on 4/17/17.
//
//

#ifndef deadtimeprocess_h
#define deadtimeprocess_h

#include <vector>
#include <Daq/Mapping.h>
#include <Daq/Process.h>

class ProcessStats;

class DeadtimeProcess : public Process {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;
    using DetIdT = Process::DetIdT;
    using IdLookupT = Mapping::IdLookupT;

    DeadtimeProcess(const IdLookupT& lookup, TimeT deadtime,
                    bool paralyzable);
    EventIter process(
            EventIter begin, EventIter end,
            ProcessStats& stats) const final;

private:
    DetIdT mapped_id(const EventT& event) const;

    /*!
     * A lookup table for the component id that is associated with each
     * detector id.
     */
    const IdLookupT id_lookup;
    TimeT time_window;
    bool is_paralyzable;
};
#endif // deadtimeprocess_h
