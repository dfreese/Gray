//
//  singlesproc.h
//  graypipeline
//
//  Created by David Freese on 4/1/17.
//
//

#ifndef processstream_h
#define processstream_h

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

    void add_process(Processor<EventT> * process) {
        processes.push_back(process);
    }

    size_t no_processes() const {
        return(processes.size());
    }

    /*!
     * Adds a new event into the merge map.  It then updates which events are
     * timed out, and will not be merged with any other event.
     */
    void add_event(const EventT & event) {
        // Currently don't do any bounds checking, assuming the user will have
        // added at least one process.
        processes[0]->add_event(event);
        cascade_processes();
    }

    void add_events(const std::vector<EventT> & events) {
        // Currently don't do any bounds checking, assuming the user will have
        // added at least one process.
        processes[0]->add_events(events);
        cascade_processes();
    }

    void stop() {
        for (auto & proc: processes) {
            proc->stop();
            // TODO: this is a bit of a hack, instead of appropriately stopping
            // and handling each layer.
            cascade_processes();
        }
    }

    const std::vector<EventT> & get_ready() const {
        return(ready_events);
    }

    size_t no_ready() const {
        return(ready_events.size());
    }

    void clear() {
        ready_events.clear();
    }

    void reset() {
        for (auto & p: processes) {
            p.reset();
        }
        ready_events.clear();
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

private:

    std::vector<Processor<EventT> *> processes;
    std::vector<EventT> ready_events;

    void cascade_processes() {
        for (size_t pidx = 0; pidx < processes.size() - 1; ++pidx) {
            Processor<EventT> * cur_process = processes[pidx];
            Processor<EventT> * next_process = processes[pidx + 1];
            const std::vector<EventT> & events = cur_process->get_ready();
            for (size_t eidx = 0; eidx < cur_process->no_ready(); ++eidx) {
                next_process->add_event(events[eidx]);
            }
            cur_process->clear();
        }

        Processor<EventT> * last_process = processes.back();
        const std::vector<EventT> & events = last_process->get_ready();
        ready_events.insert(ready_events.end(), events.begin(),
                            events.begin() + last_process->no_ready());
        last_process->clear();
    }

};

#endif // singlesproc_h
