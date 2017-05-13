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
#include <Pipeline/paralleloutstream.h>
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
    size_t no_coinc_processes() const;
    std::vector<EventT> get_coinc_buffer(size_t idx);
    std::vector<EventT> add_event(const EventT & event);
    std::vector<EventT> add_events(const std::vector<EventT> & events);
    std::vector<EventT> stop();
    long no_events() const;
    long no_kept() const;
    long no_dropped() const;
    long no_merged() const;
    long no_filtered() const;
    long no_coinc_dropped() const;
    friend std::ostream & operator << (std::ostream & os,
                                       const InteractionStream & s);

private:
    typedef std::function<TimeT(const EventT&, const EventT&)> TimeDiffF;
    typedef std::function<int(const EventT&)> InfoF;
    typedef std::function<bool(const EventT&)> FilterF;
    typedef std::function<void(EventT&)> BlurF;
    typedef std::function<TimeT(const EventT&)> TimeF;
    typedef std::function<void(EventT&, const EventT&)> MergeF;

    ParallelOutStream<EventT> process_stream;
    typedef MergeProcess<EventT, TimeT, TimeF, InfoF, MergeF> MergeProcT;
    typedef FilterProcess<EventT, FilterF> FilterProcT;
    typedef BlurProcess<EventT, BlurF> BlurProcT;
    typedef SortProcess<EventT, TimeT, TimeF> SortProcT;
    typedef CoincProcess<EventT, TimeT, TimeF> CoincProcT;
    typedef DeadtimeProcess<EventT, TimeT, TimeF> DeadtimeT;
    std::map<std::string, std::vector<int>> id_maps;
    std::vector<MergeProcT*> merge_processes;
    std::vector<FilterProcT*> filter_processes;
    std::vector<BlurProcT*> blur_processes;
    std::vector<SortProcT*> sort_processes;
    std::vector<CoincProcT*> coinc_processes;
    std::vector<DeadtimeT*> deadtime_processes;

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

    MergeF make_anger_func(const std::vector<std::string> & block_maps,
                           const std::vector<int> & block_size);

    int make_anger_func(const std::vector<std::string> & anger_opts,
                        MergeF & merge_func);

    /*!
     * merge_info_func handles all of the information besides the assignment of
     * the detector id when two events are merged together.  This can be
     * specifed, or will be the sum of the energies as a default.
     */
    MergeF merge_info_func;

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
};

#endif // interactionstream_h
