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
#include <Daq/ProcessFactory.h>

class InteractionStream {
public:
    using EventT = Process::EventT;
    using ContainerT = Process::ContainerT;
    using EventIter = Process::EventIter;
    using TimeT = Process::TimeT;
    using DetIdT = Process::DetIdT;

    InteractionStream(TimeT initial_sort_window = -1);

    ContainerT& get_buffer();

    void set_mappings(const std::map<std::string, std::vector<DetIdT>> & maps);
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
    std::vector<std::unique_ptr<Process>> coinc_processes;

    //! Tells if a given process in processes should be printed
    std::vector<bool> print_info;

    using ProcessDescription = ProcessFactory::ProcessDescription;

    int set_processes(const std::vector<ProcessDescription> & process_descriptions);

    void add_process(std::unique_ptr<Process> process, bool proc_print_info);

    ContainerT input_events;
    std::vector<ContainerT::difference_type> process_ready_distance;
    ContainerT::difference_type min_coinc_ready_dist;
    EventIter singles_ready;
    EventIter coinc_ready;
    EventIter begin();
    EventIter end();
    bool hits_stopped = false;
    bool singles_stopped = false;
    bool coinc_stopped = false;
};

#endif // interactionstream_h
