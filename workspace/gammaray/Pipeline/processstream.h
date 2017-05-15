//
//  singlesproc.h
//  graypipeline
//
//  Created by David Freese on 4/1/17.
//
//

#ifndef processstream_h
#define processstream_h

#include <ostream>
#include <vector>
#include <Pipeline/processor.h>

/*!
 * Creates a base class for events rolling merge of a user specified type.
 */
template <class EventT>
class ProcessStream {
public:
    /*!
     * Basic constructor
     */
    ProcessStream() {}
    virtual ~ProcessStream() {}

    void add_process(Processor<EventT> * process, bool print = true) {
        processes.push_back(process);
        print_info.push_back(print);
    }

    size_t no_processes() const {
        return(processes.size());
    }

    /*!
     * Adds a new event into the merge map.  It then updates which events are
     * timed out, and will not be merged with any other event.
     */
    std::vector<EventT> add_event(const EventT & event) {
        return(add_events({event}));
    }

    virtual std::vector<EventT> add_events(const std::vector<EventT> & events) {
        std::vector<EventT> output_events(events);
        for (auto proc_ptr: processes) {
            output_events = proc_ptr->add_events(output_events);
        }
        return(output_events);
    }

    virtual std::vector<EventT> stop() {
        std::vector<EventT> ret_events;
        for (size_t idx = 0; idx < processes.size(); ++idx) {
            std::vector<EventT> store_events = processes[idx]->stop();
            for (size_t pidx = idx + 1; pidx < processes.size(); ++pidx) {
                store_events = processes[pidx]->add_events(store_events);
            }
            ret_events.insert(ret_events.end(), store_events.begin(),
                              store_events.end());
        }
        return(ret_events);
    }

    virtual void reset() {
        for (auto & p: processes) {
            p->reset();
        }
    }

    long no_events() const {
        // Since every event goes through the first process, just return it's
        // stats.
        return(processes.front()->no_events());
    }

    long no_kept() const {
        // Since every event that is kept goes through the last process,
        // just return that processes' stats.
        return(processes.back()->no_kept());
    }

    long no_dropped() const {
        long dropped = 0;
        for (auto & p: processes) {
            dropped += p->no_dropped();
        }
        return(dropped);
    }

    friend std::ostream & operator << (std::ostream & os,
                                       const ProcessStream<EventT> & ps)
    {
        os << "events: " << ps.no_events() << "\n"
        << "kept: " << ps.no_kept() << "\n"
        << "dropped: " << ps.no_dropped() << "\n"
        << "drop per level: ";
        for (size_t idx = 0; idx < ps.processes.size(); idx++) {
            if (ps.print_info[idx]) {
                os << " " << ps.processes[idx]->no_dropped() << ",";
            }
        }
        os << "\n";
        return(os);
    }

private:
    std::vector<Processor<EventT> *> processes;
    std::vector<bool> print_info;
};

#endif // singlesproc_h
