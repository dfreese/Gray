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
#include <sstream>
#include <vector>
#include <Physics/Interaction.h>
#include <Pipeline/processor.h>
#include <Pipeline/blur.h>
#include <Pipeline/blurprocess.h>
#include <Pipeline/coincprocess.h>
#include <Pipeline/deadtimeprocess.h>
#include <Pipeline/mergeprocess.h>
#include <Pipeline/filterprocess.h>
#include <Pipeline/sortprocess.h>

class InteractionStream {
public:
    typedef Interaction EventT;
    typedef decltype(EventT::time) TimeT;

    InteractionStream(TimeT initial_sort_window = -1);
    ~InteractionStream();

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

    std::vector<Interaction> & get_buffer() {
        return (input_events);
    }

    std::vector<Interaction>::iterator hits_begin();
    std::vector<Interaction>::iterator hits_end();
    std::vector<Interaction>::iterator singles_begin();
    std::vector<Interaction>::iterator singles_end();
    std::vector<Interaction>::iterator coinc_begin();
    std::vector<Interaction>::iterator coinc_end();


    void process_hits();
    void process_singles();
    void process_coinc(size_t idx);

    void stop_hits();
    void stop_singles();
    void stop_coinc(size_t idx);

    void clear_complete();


private:
    typedef std::function<TimeT(const EventT&, const EventT&)> TimeDiffF;
    typedef std::function<int(const EventT&)> InfoF;
    typedef std::function<bool(EventT&)> FilterF;
    typedef std::function<void(EventT&)> BlurF;
    typedef std::function<TimeT(const EventT&)> TimeF;
    typedef std::function<void(EventT&, const EventT&)> MergeF;

    typedef Processor<EventT> ProcT;
    typedef MergeProcess<EventT, TimeT, TimeF, InfoF, MergeF> MergeProcT;
    typedef FilterProcess<EventT, FilterF> FilterProcT;
    typedef BlurProcess<EventT, BlurF> BlurProcT;
    typedef SortProcess<EventT, TimeT, TimeF> SortProcT;
    typedef CoincProcess<EventT, TimeT, TimeF> CoincProcT;
    typedef DeadtimeProcess<EventT, TimeT, TimeF> DeadtimeT;
    std::map<std::string, std::vector<int>> id_maps;

    std::vector<ProcT*> processes;
    std::vector<MergeProcT*> merge_processes;
    std::vector<FilterProcT*> filter_processes;
    std::vector<BlurProcT*> blur_processes;
    std::vector<SortProcT*> sort_processes;
    std::vector<CoincProcT*> coinc_processes;
    std::vector<DeadtimeT*> deadtime_processes;

    //! Tells if a given process in processes should be printed
    std::vector<bool> print_info;

    struct MergeFirstFunctor;
    struct MergeMaxFunctor;
    struct MergeAngerLogicFunctor;
    struct FilterEnergyGateLowFunctor;
    struct FilterEnergyGateHighFunctor;

    static int load_id_maps(const std::string & filename,
                            std::map<std::string, std::vector<int>> & id_maps);

    struct ProcessDescription {
        std::string type;
        std::string component;
        double time;
        std::vector<std::string> options;
    };

    static int line_to_process_description(const std::string & line,
                                           ProcessDescription & proc_desc);

    static int load_process_list(
            const std::string & filename,
            std::vector<ProcessDescription> & process_descriptions);
    static int convert_process_lines(
            const std::vector<std::string> & lines,
            std::vector<ProcessDescription> & process_descriptions);

    int set_processes(const std::vector<ProcessDescription> & process_descriptions);

    void add_process(ProcT * process, bool proc_print_info);

    int make_anger_func(const std::string & map_name,
                        const std::vector<std::string> & anger_opts,
                        MergeF & merge_func);

    /*!
     * Returns the detector id for the event.  This is then mapped to a.
     */
    InfoF get_id_func;

    TimeF get_time_func;

    int add_merge_process(const std::string & map_name, double merge_time,
                          const std::vector<std::string> & options);

    int add_filter_process(const std::string & filter_name, double value);

    int add_blur_process(const std::string & name, double value,
                         const std::vector<std::string> & options);

    int add_sort_process(const std::string & name, double value,
                         const std::vector<std::string> & options,
                         bool user_requested);

    int add_coinc_process(const std::string & name, double value,
                          const std::vector<std::string> & options);

    int add_deadtime_process(const std::string & map_name, double deadtime,
                             const std::vector<std::string> & options);

    std::vector<Interaction> input_events;
    std::vector<std::vector<Interaction>::difference_type> process_ready_distance;
    std::vector<Interaction>::iterator singles_ready;
    std::vector<Interaction>::difference_type min_coinc_ready_dist;
    std::vector<Interaction>::iterator coinc_ready;
    bool hits_stopped = false;
    bool singles_stopped = false;
    bool coinc_stopped = false;
    std::vector<Interaction>::iterator begin();
    std::vector<Interaction>::iterator end();
};

#endif // interactionstream_h
