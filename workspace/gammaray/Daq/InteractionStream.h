//
//  InteractionStream.h
//  graypipeline
//
//  Created by David Freese on 4/1/17.
//
//

#ifndef interactionstream_h
#define interactionstream_h

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include <Physics/Interaction.h>
#include <Daq/Process.h>
#include <Daq/CoincProcess.h>
#include <Daq/MergeProcess.h>

class InteractionStream {
public:
    using EventT = Process::EventT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;
    using DetIdT = Process::DetIdT;

    InteractionStream(TimeT initial_sort_window = -1);

    void set_mappings(const std::map<std::string, std::vector<int>> & mapping);
    int load_mappings(const std::string & filename);
    int set_processes(const std::vector<std::string> & lines);
    int load_processes(const std::string & filename);
    size_t no_processes() const;
    size_t no_coinc_processes() const;
    long no_events() const;
    long no_kept() const;
    long no_dropped() const;
    long no_merged() const;
    long no_filtered() const;
    long no_deadtimed() const;
    friend std::ostream & operator << (std::ostream & os,
                                       const InteractionStream & s);

    std::vector<EventT> & get_buffer() {
        return (input_events);
    }

    EventIter hits_begin();
    EventIter hits_end();
    EventIter singles_begin();
    EventIter singles_end();
    EventIter coinc_begin();
    EventIter coinc_end();


    void process_hits();
    void process_singles();
    void process_coinc(size_t idx);

    void stop_hits();
    void stop_singles();
    void stop_coinc(size_t idx);

    void clear_complete();


private:

    std::map<std::string, std::vector<DetIdT>> id_maps;

    std::vector<std::unique_ptr<Process>> processes;
    std::vector<std::unique_ptr<CoincProcess>> coinc_processes;

    //! Tells if a given process in processes should be printed
    std::vector<bool> print_info;

    struct MergeFirstFunctor;
    struct MergeMaxFunctor;
    struct MergeAngerLogicFunctor;
    struct FilterEnergyGateLowFunctor;
    struct FilterEnergyGateHighFunctor;
    struct BlurEnergyFunctor;
    struct BlurEnergyReferencedFunctor;
    struct BlurTimeFunctor;

    static int load_id_maps(const std::string & filename,
                            std::map<std::string,
                            std::vector<DetIdT>> & id_maps);

    struct ProcessDescription {
        std::string type;
        std::vector<std::string> args;
        bool as_double(size_t idx, double & val) {
            try {
                val = std::stod(args.at(idx));
                return (true);
            } catch (...) {
                return (false);
            }
        }
    };

    static int line_to_process_description(const std::string & line,
                                           ProcessDescription & desc);

    static int load_process_list(
            const std::string & filename,
            std::vector<ProcessDescription> & process_descriptions);
    static int convert_process_lines(
            const std::vector<std::string> & lines,
            std::vector<ProcessDescription> & process_descriptions);

    int set_processes(const std::vector<ProcessDescription> & process_descriptions);

    void add_process(std::unique_ptr<Process> process, bool proc_print_info);

    int make_anger_func(const std::string & map_name,
                        const std::vector<std::string> & anger_opts,
                        MergeProcess::MergeF & merge_func);

    int add_merge_process(ProcessDescription desc);
    int add_filter_process(ProcessDescription desc);
    int add_blur_process(ProcessDescription desc);
    int add_sort_process(ProcessDescription desc, bool user_requested);
    int add_coinc_process(ProcessDescription desc);
    int add_deadtime_process(ProcessDescription desc);

    std::vector<EventT> input_events;
    std::vector<std::vector<EventT>::difference_type> process_ready_distance;
    std::vector<EventT>::difference_type min_coinc_ready_dist;
    EventIter singles_ready;
    EventIter coinc_ready;
    EventIter begin();
    EventIter end();
    bool hits_stopped = false;
    bool singles_stopped = false;
    bool coinc_stopped = false;
};

#endif // interactionstream_h
