//
//  singlesproc.h
//  graypipeline
//
//  Created by David Freese on 4/1/17.
//
//

#ifndef paralleloutstream_h
#define paralleloutstream_h

#include <ostream>
#include <utility>
#include <vector>
#include <Pipeline/processstream.h>

/*!
 * Adds an additional layer onto ProcessStream so that the last stage of
 * ProcessStream can be fanned-out into multple different processes.  Mainly
 * useful for multiple coincidence sorting stages that could be used, such as
 * a main coincidence window and a delayed coincidence window.
 */
template <class EventT>
class ParallelOutStream : public ProcessStream<EventT> {
public:
    ~ParallelOutStream() final = default;

    void add_parallel_out_process(Processor<EventT> * process) {
        proc_pair.push_back({process, std::vector<EventT>()});
    }

    size_t no_parallel_processes() const {
        return(proc_pair.size());
    }

    std::vector<EventT> add_events(const std::vector<EventT> & events) override
    {
        auto result = ProcessStream<EventT>::add_events(events);
        for (auto & pp: proc_pair) {
            std::vector<EventT> parallel_result = pp.first->add_events(result);
            pp.second.insert(pp.second.end(), parallel_result.begin(),
                             parallel_result.end());
        }
        return(result);
    }

    std::vector<EventT> stop() override {
        auto result = ProcessStream<EventT>::stop();
        for (auto & pp: proc_pair) {
            // First add the events to the process, like normal
            std::vector<EventT> parallel_result = pp.first->add_events(result);
            pp.second.insert(pp.second.end(), parallel_result.begin(),
                             parallel_result.end());
            // Then stop the process and append the output
            parallel_result = pp.first->stop();
            pp.second.insert(pp.second.end(), parallel_result.begin(),
                             parallel_result.end());
        }
        return(result);
    }

    void reset() override {
        for (auto pp: proc_pair) {
            pp.first->reset();
        }
        ProcessStream<EventT>::reset();
    }

    void clear() {
        for (auto pp: proc_pair) {
            pp.second.clear();
        }
    }

    std::vector<EventT> get_buffer(size_t idx) {
        std::vector<EventT> tmp;
        tmp.swap(proc_pair.at(idx).second);
        return(tmp);
    }

private:
    std::vector<std::pair<Processor<EventT>*, std::vector<EventT>>> proc_pair;
};

#endif // paralleloutstream_h
