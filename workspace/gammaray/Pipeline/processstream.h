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
    virtual ~ProcessStream() = default;

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
        count_no_events += events.size();
        std::vector<EventT> output_events(events);
        for (auto proc_ptr: processes) {
            output_events = proc_ptr->add_events(output_events);
        }
        count_no_kept += output_events.size();
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
        count_no_kept += ret_events.size();
        return(ret_events);
    }

    virtual void reset() {
        for (auto & p: processes) {
            p->reset();
        }
        count_no_kept = 0;
        count_no_events = 0;
    }

    long no_events() const {
        return(count_no_events);
    }

    long no_kept() const {
        return(count_no_kept);
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
    long count_no_events = 0;
    long count_no_kept = 0;
};

#endif // singlesproc_h
