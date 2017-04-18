//
//  singlesstream.h
//  graypipeline
//
//  Created by David Freese on 4/1/17.
//
//

#ifndef singlesstream_h
#define singlesstream_h

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <Pipeline/processstream.h>
#include <Pipeline/blur.h>
#include <Pipeline/blurprocess.h>
#include <Pipeline/coincprocess.h>
#include <Pipeline/deadtimeprocess.h>
#include <Pipeline/mergeprocess.h>
#include <Pipeline/filterprocess.h>
#include <Pipeline/sortprocess.h>

/*!
 * Creates a base class for events rolling merge of a user specified type.
 */
template <class EventT>
class SinglesStream {
public:
    typedef decltype(EventT::time) TimeT;
    /*!
     * If the initial sort window is greater than zero, a sorting process is
     * added to the stream immediately with that wait window.  Necessary for
     * gray, where we need to guarantee sorting before the user's preferences
     * are added.
     */
    SinglesStream(TimeT initial_sort_window = -1) :
        id_func([](const EventT & e){return (e.det_id);}),
        deltat_func([](const EventT & e0,
                       const EventT & e1)
                    {return (e0.time - e1.time);}),
        get_time_func([](const EventT & e){return (e.time);}),
        time_less_than(TimeCompF())
    {
        if (initial_sort_window > 0) {
            add_sort_process("time", initial_sort_window,
                             std::vector<std::string>());
        }
        // Func to merge events in an array, and place the event at the first.
        merge_types["merge_first"] = [](EventT & e0, const EventT & e1) {
            e0.energy += e1.energy;
        };
        // Func to merge events, and place the event at the max energy.
        merge_types["merge_max"] = [](EventT & e0, const EventT & e1) {
            e0.det_id = e0.energy > e1.energy ? e0.det_id:e1.det_id;
            e0.energy += e1.energy;
        };
    }

    ~SinglesStream() {
        for (auto p: merge_processes) {
            delete p;
        }
        for (auto p: filter_processes) {
            delete p;
        }
        for (auto p: blur_processes) {
            delete p;
        }
        for (auto p: sort_processes) {
            delete p;
        }
        for (auto p: coinc_processes) {
            delete p;
        }
        for (auto p: deadtime_processes) {
            delete p;
        }
    }

    void set_mappings(const std::map<std::string, std::vector<int>> & mapping) {
        this->id_maps = mapping;
    }

    int load_mappings(const std::string & filename) {
        std::map<std::string, std::vector<int>> id_maps;
        int no_detectors = load_id_maps(filename, id_maps);
        if (no_detectors < 0) {
            return(-1);
        }
        this->id_maps = id_maps;
        return(no_detectors);

    }

    int set_processes(const std::vector<std::string> & lines) {
        std::vector<ProcessDescription> process_descriptions;
        int convert_status = convert_process_lines(lines, process_descriptions);
        if (convert_status < 0) {
            return(-1);
        }

        return(set_processes(process_descriptions));
    }

    int load_processes(const std::string & filename) {
        std::vector<ProcessDescription> process_descriptions;
        int proc_load_status = load_process_list(filename,
                                                 process_descriptions);
        if (proc_load_status < 0) {
            return(-1);
        }

        return(set_processes(process_descriptions));
    }

    void add_event(const EventT & event) {
        process_stream.add_event(event);
    }

    void add_events(const std::vector<EventT> & events) {
        process_stream.add_events(events);
    }


    long no_ready() const {
        return(process_stream.no_ready());
    }

    const std::vector<EventT> & get_ready() const {
        return(process_stream.get_ready());
    }

    void clear() {
        process_stream.clear();
    }

    void stop() {
        process_stream.stop();
    }

    long no_events() const {
        return(process_stream.no_events());
    }

    long no_kept() const {
        return(process_stream.no_kept());
    }

    long no_dropped() const {
        return(process_stream.no_dropped());
    }

    long no_merged() const {
        long merged = 0;
        for (auto & proc: merge_processes) {
            merged += proc->no_dropped();
        }
        return(merged);
    }

    long no_filtered() const {
        long filtered = 0;
        for (auto & proc: filter_processes) {
            filtered += proc->no_dropped();
        }
        return(filtered);
    }

    long no_coinc_dropped() const {
        long coinc_dropped = 0;
        for (auto & proc: coinc_processes) {
            coinc_dropped += proc->no_dropped();
        }
        return(coinc_dropped);
    }

    friend std::ostream & operator << (std::ostream & os,
                                       const SinglesStream<EventT> & s)
    {
        os << s.process_stream;
        if (s.merge_processes.size()) {
            os << "merged: " << s.no_merged() << "\n";
        }
        if (s.filter_processes.size()) {
            os << "filtered: " << s.no_filtered() << "\n";
        }
        if (s.coinc_processes.size()) {
            os << "coinc windowed: " << s.no_coinc_dropped() << "\n";
            for (auto & proc: s.coinc_processes) {
                os << *proc;
            }
        }
        return(os);
    }

private:
    typedef std::function<TimeT(const EventT&, const EventT&)> TimeDiffF;
    typedef std::function<int(const EventT&)> InfoF;
    typedef std::less<TimeT> TimeCompF;
    typedef std::function<void(EventT&, const EventT&)> MergeF;
    typedef std::function<bool(const EventT&)> FilterF;
    typedef std::function<void(EventT&)> BlurF;
    typedef std::function<TimeT(const EventT&)> TimeF;

    ProcessStream<EventT> process_stream;
    typedef MergeProcess<EventT, TimeT, TimeDiffF, InfoF, TimeCompF, MergeF> MergeProcT;
    typedef FilterProcess<EventT, FilterF> FilterProcT;
    typedef BlurProcess<EventT, BlurF> BlurProcT;
    typedef SortProcess<EventT, TimeT, TimeDiffF, TimeCompF> SortProcT;
    typedef CoincProcess<EventT, TimeT, TimeDiffF, TimeCompF> CoincProcT;
    typedef DeadtimeProcess<EventT, TimeT, TimeF> DeadtimeT;
    std::map<std::string, std::vector<int>> id_maps;
    std::vector<MergeProcT*> merge_processes;
    std::vector<FilterProcT*> filter_processes;
    std::vector<BlurProcT*> blur_processes;
    std::vector<SortProcT*> sort_processes;
    std::vector<CoincProcT*> coinc_processes;
    std::vector<DeadtimeT*> deadtime_processes;
    std::map<std::string, MergeF> merge_types;

    static int load_id_maps(const std::string & filename,
                            std::map<std::string, std::vector<int>> & id_maps)
    {
        std::ifstream input(filename);
        if (!input) {
            return(-1);
        }
        std::string headers;
        getline(input, headers);
        std::stringstream head_ss(headers);
        std::string header;
        std::vector<std::string> header_vec;
        while (head_ss >> header) {
            header_vec.push_back(header);
            id_maps[header] = std::vector<int>();
        }

        std::string line;
        int no_detectors = 0;
        while (getline(input, line)) {
            std::stringstream line_ss(line);
            for (const auto & header: header_vec) {
                int val;
                if ((line_ss >> val).fail()) {
                    return(-2);
                }
                id_maps[header].push_back(val);
            }
            no_detectors++;
        }
        return(no_detectors);
    }

    struct ProcessDescription {
        std::string type;
        std::string component;
        double time;
        std::vector<std::string> options;
    };

    static int line_to_process_description(const std::string & line,
                                           ProcessDescription & proc_desc)
    {

        // Ignore blank lines, including just all whitespace
        if (line.find_first_not_of(" ") == std::string::npos) {
            return(0);
        }
        // Remove leading spaces, and anything after a comment
        std::string new_line = line.substr(line.find_first_not_of(" "),
                                           line.find_first_of("#"));
        // Ignore blank lines again after removing comments
        if (new_line.empty()) {
            return(0);
        }

        std::stringstream line_ss(new_line);
        if ((line_ss >> proc_desc.type).fail()) {
            return(-1);
        }

        if ((line_ss >> proc_desc.component).fail()) {
            return(-1);
        }

        if ((line_ss >> proc_desc.time).fail()) {
            return(-1);
        }

        std::string txt_val;
        while (line_ss >> txt_val) {
            proc_desc.options.push_back(txt_val);
        }
        return(1);
    }

    static int load_process_list(
            const std::string & filename,
            std::vector<ProcessDescription> & process_descriptions)
    {
        std::ifstream input(filename);
        if (!input) {
            return(-1);
        }
        std::string line;
        while (getline(input, line)) {
            ProcessDescription proc_desc;
            int status = line_to_process_description(line, proc_desc);
            if (status < 0) {
                return(status);
            } else if (status == 0) {
                // A blank line that we can safely ignore.
                continue;
            } else {
                process_descriptions.push_back(proc_desc);
            }
        }
        return(0);
    }

    static int convert_process_lines(
            const std::vector<std::string> & lines,
            std::vector<ProcessDescription> & process_descriptions)
    {
        for (const auto & line: lines) {
            ProcessDescription proc_desc;
            int status = line_to_process_description(line, proc_desc);
            if (status < 0) {
                return(status);
            } else if (status == 0) {
                // A blank line that we can safely ignore.
                continue;
            } else {
                process_descriptions.push_back(proc_desc);
            }
        }
    }


    int set_processes(const std::vector<ProcessDescription> & process_descriptions) {
        for (const auto & proc_desc: process_descriptions) {
            if (proc_desc.type == "merge") {
                if (proc_desc.options.empty()) {
                    if (add_merge_process("merge_max", proc_desc.component,
                                          proc_desc.time) < 0)
                    {
                        return(-3);
                    }
                } else if (proc_desc.options[0] == "first") {
                    if (add_merge_process("merge_first", proc_desc.component,
                                          proc_desc.time) < 0)
                    {
                        return(-3);
                    }
                } else if (proc_desc.options[0] == "max") {
                    if (add_merge_process("merge_max", proc_desc.component,
                                          proc_desc.time) < 0)
                    {
                        return(-3);
                    }
                } else if (proc_desc.options[0] == "array_eweight") {
                    // Give each eweight process a unique name, so that is can
                    // have unique settings.
                    std::stringstream ss("merge_array_eweight_");
                    ss << no_array_eweight++;
                    add_array_eweight_func(proc_desc.options, ss.str());
                    if (add_merge_process(ss.str(), proc_desc.component,
                                          proc_desc.time) < 0)
                    {
                        return(-3);
                    }
                } else {
                    return(-4);
                }
            } else if (proc_desc.type == "deadtime") {
                if (add_deadtime_process(proc_desc.component, proc_desc.time,
                                         proc_desc.options) < 0)
                {
                    return(-3);
                }
            } else if (proc_desc.type == "filter") {
                if (add_filter_process(proc_desc.component, proc_desc.time) < 0)
                {
                    return(-3);
                }
            } else if (proc_desc.type == "blur") {
                if (add_blur_process(proc_desc.component, proc_desc.time,
                                     proc_desc.options) < 0)
                {
                    return(-3);
                }
            } else if (proc_desc.type == "sort") {
                if (add_sort_process(proc_desc.component, proc_desc.time,
                                     proc_desc.options) < 0)
                {
                    return(-3);
                }
            } else if (proc_desc.type == "coinc") {
                if (add_coinc_process(proc_desc.component, proc_desc.time,
                                      proc_desc.options) < 0)
                {
                    return(-3);
                }
            } else {
                std::cerr << "Process Type not supported: " << proc_desc.type
                << std::endl;
                return(-2);
            }
        }
        return(0);
    }


    void add_array_eweight_func(const std::vector<std::string> & block_maps,
                                const std::vector<int> & block_size,
                                const std::string & merge_name)
    {
        // TODO: make sure these mappings actually exist, and if not create
        // reasonable defaults or throw and error.
        const std::vector<int> & bx = this->id_maps[block_maps[0]];
        const std::vector<int> & by = this->id_maps[block_maps[1]];
        const std::vector<int> & bz = this->id_maps[block_maps[2]];
        const int no_by = block_size[1];
        const int no_bz = block_size[2];
        auto merge_array_eweight = [bx, by, bz, no_by, no_bz](EventT & e0,
                                                              const EventT & e1)
        {
            float energy_result = e0.energy + e1.energy;
            int row0 = bx[e0.det_id];
            int row1 = bx[e1.det_id];
            int col0 = by[e0.det_id];
            int col1 = by[e1.det_id];
            int lay0 = bz[e0.det_id];
            int lay1 = bz[e1.det_id];
            int row_result = static_cast<int>(
                    static_cast<float>(row0) * (e0.energy / energy_result) +
                    static_cast<float>(row1) * (e1.energy / energy_result));
            int col_result = static_cast<int>(
                    static_cast<float>(col0) * (e0.energy / energy_result) +
                    static_cast<float>(col1) * (e1.energy / energy_result));
            int lay_result = static_cast<int>(
                    static_cast<float>(lay0) * (e0.energy / energy_result) +
                    static_cast<float>(lay1) * (e1.energy / energy_result));

            // TODO: lookup the resulting det_id, rather than assume it's all
            // linear like this.
            e0.det_id -= (row0 * no_by + col0) * no_bz + lay0;
            e0.det_id += (row_result * no_by + col_result) * no_bz + lay_result;
            e0.energy = energy_result;
        };
        merge_types[merge_name] = merge_array_eweight;
    }

    int add_array_eweight_func(const std::vector<std::string> & eweight_opts,
                               const std::string & merge_name)
    {
        if (eweight_opts.size() != 7) {
            return(-1);
        }
        std::vector<std::string> block_maps(3, "");
        std::vector<int> block_size(3, 0);
        // The first one should be "array_eweight"
        for (size_t idx = 0; idx < 3; idx++) {
            block_maps[idx] = eweight_opts[idx + 1];
            std::stringstream ss(eweight_opts[idx + 4]);
            if ((ss >> block_size[idx]).fail()) {
                return(-2);
            }
        }
        for (const auto & bm: block_maps) {
            if (!this->id_maps.count(bm)) {
                std::cerr << "Block label not found: " << bm << std::endl;
                return(-3);
            }
        }
        add_array_eweight_func(block_maps, block_size, merge_name);
        return(0);
    }

    int no_array_eweight;

    /*!
     * Returns the detector id for the event.  This is then mapped to a.
     */
    InfoF id_func;

    /*!
     * A function type that calculates the time difference between two events.
     * First - Second.
     */
    TimeDiffF deltat_func;

    TimeF get_time_func;

    TimeCompF time_less_than;

    int add_merge_process(const std::string & merge_name,
                           const std::string & map_name,
                           TimeT merge_time)
    {
        if (merge_types.count(merge_name) == 0) {
            std::cerr << "Unknown process type: " << merge_name << std::endl;
            return(-1);
        }
        if (id_maps.count(map_name) == 0) {
            std::cerr << "Unknown id map type: " << map_name << std::endl;
            return(-2);
        }
        MergeF merge_func = merge_types[merge_name];
        const std::vector<int> id_map = id_maps[map_name];
        merge_processes.push_back(new MergeProcT(id_map, merge_time,
                                                 deltat_func, id_func,
                                                 merge_func, time_less_than));
        process_stream.add_process(merge_processes.back());
        return(0);
    }

    int add_filter_process(const std::string & filter_name, double value)
    {
        FilterF filt_func;
        if (filter_name == "egate_low") {
            filt_func = [value](const EventT & e) {
                return(e.energy >= value);
            };
        } else if (filter_name == "egate_high") {
            filt_func = [value](const EventT & e) {
                return(e.energy <= value);
            };
        } else {
            std::cerr << "Unknown filter type: " << filter_name << std::endl;
            return(-1);
        }
        filter_processes.push_back(new FilterProcT(filt_func));
        process_stream.add_process(filter_processes.back());
        return(0);
    }

    int add_blur_process(const std::string & name, double value,
                         const std::vector<std::string> & options)
    {
        if (name == "energy") {
            if (options.empty()) {
                auto eblur = [value](EventT & e) {
                    Blur::blur_energy(e, value);
                };
                blur_processes.push_back(new BlurProcT(eblur));
                process_stream.add_process(blur_processes.back());
                return(0);
            } else if (options[0] == "at") {
                double ref_energy;
                std::stringstream ss(options[1]);
                if ((ss >> ref_energy).fail()) {
                    std::cerr << "invalid reference energy: " << options[1]
                              << std::endl;
                }
                auto eblur = [value, ref_energy](EventT & e) {
                    Blur::blur_energy_invsqrt(e, value, ref_energy);
                };
                blur_processes.push_back(new BlurProcT(eblur));
                process_stream.add_process(blur_processes.back());
                return(0);
            } else {
                std::cerr << "unrecognized blur option: " << options[0]
                          << std::endl;
                return(-1);
            }
        } else if (name == "time") {
            // Allow the value to be 3 FWHM on either side of the current event
            // TODO: allow this to be set by options.
            TimeT max_blur = 3 * value;
            BlurF tblur = [value, max_blur](EventT & e) {
                Blur::blur_time_capped(e, value, max_blur);
            };
            blur_processes.push_back(new BlurProcT(tblur));
            process_stream.add_process(blur_processes.back());

            // Sort the stream afterwards, based on the capped blur
            sort_processes.push_back(new SortProcT(max_blur * 2, deltat_func,
                                                   time_less_than));
            process_stream.add_process(sort_processes.back());
            return(0);
        } else {
            std::cerr << "Unknown blur type: " << name << std::endl;
            return(-1);
        }
    }

    int add_sort_process(const std::string & name, double value,
                         const std::vector<std::string> & options)
    {
        if (name == "time") {
            sort_processes.push_back(new SortProcT(value, deltat_func,
                                                   time_less_than));
            process_stream.add_process(sort_processes.back());
            return(0);
        } else {
            std::cerr << "Unknown sort type: " << name << std::endl;
            return(-1);
        }
    }

    int add_coinc_process(const std::string & name, double value,
                          const std::vector<std::string> & options)
    {
        bool paralyzable = false;
        bool reject_multiples = true;
        bool enable_delayed_window = false;
        TimeT delayed_window_offset;
        for (size_t ii = 0; ii < options.size(); ii++) {
            const std::string & option = options[ii];
            if (option == "keep_multiples") {
                reject_multiples = false;
            } else if (option == "delay") {
                enable_delayed_window = true;
                ii++;
                if (ii == options.size()) {
                    std::cerr << "coinc delay has no time offset" << std::endl;
                    return(-1);
                }
                std::stringstream ss(options[ii]);
                if ((ss >> delayed_window_offset).fail()) {
                    std::cerr << "invalid coinc delay offset: " << ss.str()
                              << std::endl;
                    return(-1);
                }
            } else {
                std::cerr << "unrecognized coinc option: " << option
                          << std::endl;
                return(-1);
            }
        }
        if ((name == "window") || (name == "nonparalyzable")) {
            paralyzable = false;
        } else if (name == "paralyzable") {
            paralyzable = true;
        } else {
            std::cerr << "Unknown coinc type: " << name << std::endl;
            return(-2);
        }
        coinc_processes.push_back(new CoincProcT(value, deltat_func,
                reject_multiples, paralyzable, enable_delayed_window,
                delayed_window_offset, time_less_than));
        process_stream.add_process(coinc_processes.back());
        return(0);
    }


    int add_deadtime_process(const std::string & map_name, double deadtime,
                             const std::vector<std::string> & options)
    {
        bool paralyzable = false;
        for (size_t ii = 0; ii < options.size(); ii++) {
            const std::string & option = options[ii];
            if (option == "paralyzable") {
                paralyzable = true;
            } else if (option == "nonparalyzable") {
                paralyzable = false;
            } else {
                std::cerr << "unrecognized coinc option: " << option
                          << std::endl;
                return(-1);
            }
        }

        if (id_maps.count(map_name) == 0) {
            std::cerr << "Unknown id map type: " << map_name << std::endl;
            return(-2);
        }
        const std::vector<int> id_map = id_maps[map_name];
        deadtime_processes.push_back(new DeadtimeT(id_map, deadtime,
                                                   get_time_func, id_func,
                                                   paralyzable));
        process_stream.add_process(deadtime_processes.back());
        return(0);
    }
};

#endif // singlesstream_h
