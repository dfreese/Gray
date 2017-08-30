#include <Pipeline/InteractionStream.h>
#include <unordered_set>
#include <Physics/Physics.h>

/*!
 * If the initial sort window is greater than zero, a sorting process is
 * added to the stream immediately with that wait window.  Necessary for
 * gray, where we need to guarantee sorting before the user's preferences
 * are added.
 */
InteractionStream::InteractionStream(TimeT initial_sort_window) :
    get_id_func([](const EventT & e){return (e.det_id);}),
    get_time_func([](const EventT & e){return (e.time);})
{
    if (initial_sort_window > 0) {
        add_sort_process("time", initial_sort_window,
                         std::vector<std::string>(), false);
    }
}

InteractionStream::~InteractionStream() {
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

void InteractionStream::set_mappings(const std::map<std::string, std::vector<int>> & mapping) {
    this->id_maps = mapping;
}

int InteractionStream::load_mappings(const std::string & filename) {
    std::map<std::string, std::vector<int>> id_maps;
    int no_detectors = load_id_maps(filename, id_maps);
    if (no_detectors < 0) {
        return(-1);
    }
    this->id_maps = id_maps;
    return(no_detectors);

}

int InteractionStream::set_processes(const std::vector<std::string> & lines) {
    std::vector<ProcessDescription> process_descriptions;
    int convert_status = convert_process_lines(lines, process_descriptions);
    if (convert_status < 0) {
        return(-1);
    }

    return(set_processes(process_descriptions));
}

int InteractionStream::load_processes(const std::string & filename) {
    std::vector<ProcessDescription> process_descriptions;
    int proc_load_status = load_process_list(filename,
                                             process_descriptions);
    if (proc_load_status < 0) {
        return(-1);
    }

    return(set_processes(process_descriptions));
}

size_t InteractionStream::no_coinc_processes() const {
    return(process_stream.no_parallel_processes());
}

std::vector<InteractionStream::EventT> InteractionStream::get_coinc_buffer(size_t idx) {
    return(process_stream.get_buffer(idx));
}

std::vector<InteractionStream::EventT> InteractionStream::add_event(const EventT & event) {
    return(add_events({event}));
}

namespace {
struct ValidForSingles {
    bool operator()(const Interaction & i) {
        return(singles_valid_interactions.count(i.type) && (i.det_id >= 0));
    };
    std::unordered_set<int> singles_valid_interactions = {
        Physics::COMPTON,
        Physics::PHOTOELECTRIC,
        Physics::XRAY_ESCAPE,
        Physics::RAYLEIGH};
};
}

std::vector<InteractionStream::EventT> InteractionStream::add_events(
        const std::vector<EventT> & events)
{
    size_t no_valid = std::count_if(events.begin(), events.end(),
                                    ValidForSingles());
    if (no_valid < events.size()) {
        std::vector<InteractionStream::EventT> valid_events(no_valid);
        std::copy_if(events.begin(), events.end(), valid_events.begin(),
                     ValidForSingles());
        return(process_stream.add_events(valid_events));
    }
    return(process_stream.add_events(events));
}

std::vector<InteractionStream::EventT> InteractionStream::stop() {
    return(process_stream.stop());
}

long InteractionStream::no_events() const {
    return(process_stream.no_events());
}

long InteractionStream::no_kept() const {
    return(process_stream.no_kept());
}

long InteractionStream::no_dropped() const {
    return(process_stream.no_dropped());
}

long InteractionStream::no_merged() const {
    long merged = 0;
    for (auto & proc: merge_processes) {
        merged += proc->no_dropped();
    }
    return(merged);
}

long InteractionStream::no_filtered() const {
    long filtered = 0;
    for (auto & proc: filter_processes) {
        filtered += proc->no_dropped();
    }
    return(filtered);
}

std::ostream & operator << (std::ostream & os, const InteractionStream & s) {
    os << s.process_stream;
    if (s.merge_processes.size()) {
        os << "merged: " << s.no_merged() << "\n";
    }
    if (s.filter_processes.size()) {
        os << "filtered: " << s.no_filtered() << "\n";
    }
    if (s.coinc_processes.size()) {
        for (auto & proc: s.coinc_processes) {
            os << *proc;
        }
    }
    return(os);
}

int InteractionStream::load_id_maps(const std::string & filename,
                                    std::map<std::string, std::vector<int>> & id_maps)
{
    std::ifstream input(filename);
    if (!input) {
        return(-1);
    }
    std::string headers;
    // Find the first non-blank line, including comments
    while (getline(input, headers)) {
        if (headers.empty()) {
            continue;
        }
        headers = headers.substr(0, headers.find_first_of("#"));
        if (!headers.empty()) {
            break;
        }
    }
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
        if (line.empty()) {
            continue;
        }
        line = line.substr(0, line.find_first_of("#"));
        if (line.empty()) {
            continue;
        }
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

int InteractionStream::line_to_process_description(const std::string & line,
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

int InteractionStream::load_process_list(const std::string & filename,
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

int InteractionStream::convert_process_lines(
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
    return(0);
}

int InteractionStream::set_processes(
        const std::vector<ProcessDescription> & process_descriptions)
{
    // TODO: change ProcessDescription to name and options only
    for (const auto & proc_desc: process_descriptions) {
        if (proc_desc.type == "merge") {
            if (add_merge_process(proc_desc.component, proc_desc.time,
                                  proc_desc.options) < 0)
            {
                return(-3);
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
                                 proc_desc.options, true) < 0)
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

struct InteractionStream::MergeFirstFunctor {
    void operator() (EventT & e0, const EventT & e1) {
        Interaction::basic_merge(e0, e1);
    }
};

struct InteractionStream::MergeMaxFunctor {
    void operator() (EventT & e0, const EventT & e1) {
        e0.det_id = e0.energy > e1.energy ? e0.det_id:e1.det_id;
        Interaction::basic_merge(e0, e1);
    }
};

struct InteractionStream::MergeAngerLogicFunctor {
    MergeAngerLogicFunctor(const std::vector<int> & bx_map,
                           const std::vector<int> & by_map,
                           const std::vector<int> & bz_map,
                           int nx, int ny, int nz,
                           const std::vector<int> & rev_map) :
        bx(bx_map),
        by(by_map),
        bz(bz_map),
        no_bx(nx),
        no_by(ny),
        no_bz(nz),
        reverse_map(rev_map)
    {

    }

    void operator() (EventT & e0, const EventT & e1) {
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

        int rev_idx = (row_result * no_by + col_result) * no_bz + lay_result;
        e0.det_id = reverse_map[rev_idx];
        Interaction::basic_merge(e0, e1);
    }

    const std::vector<int> bx;
    const std::vector<int> by;
    const std::vector<int> bz;
    const int no_bx;
    const int no_by;
    const int no_bz;
    const std::vector<int> reverse_map;
};

int InteractionStream::make_anger_func(
        const std::vector<std::string> & anger_opts,
        MergeF & merge_func)
{
    if (anger_opts.size() != 4) {
        std::cerr << "Error: anger merge requires 3 block mapping names" << std::endl;
        return(-1);
    }
    std::vector<std::string> block_maps(3, "");
    std::vector<int> block_size(3, 0);
    // The first one should be "anger"
    for (size_t idx = 0; idx < 3; idx++) {
        block_maps[idx] = anger_opts[idx + 1];
    }
    for (const auto & bm: block_maps) {
        if (!this->id_maps.count(bm)) {
            std::cerr << "Block label not found: " << bm << std::endl;
            return(-3);
        }
    }
    const std::vector<int> & bx = this->id_maps[block_maps[0]];
    const std::vector<int> & by = this->id_maps[block_maps[1]];
    const std::vector<int> & bz = this->id_maps[block_maps[2]];
    block_size[0] = *std::max_element(bx.begin(), bx.end()) + 1;
    block_size[1] = *std::max_element(by.begin(), by.end()) + 1;
    block_size[2] = *std::max_element(bz.begin(), bz.end()) + 1;
    const int no_bx = block_size[0];
    const int no_by = block_size[1];
    const int no_bz = block_size[2];
    const int total = no_bx * no_by * no_bz;

    std::vector<int> rev_map(total, -1);
    for (int idx = 0; idx < total; idx++) {
        int rev_map_index = (bx[idx] * no_by + by[idx]) * no_bz + bz[idx];
        if ((rev_map_index < 0) || (rev_map_index >= total)) {
            std::cerr << "Block index mapping is not consistent with block size at detector "
                      << idx << std::endl;
            std::cerr << "Assuming block size of (x,y,z) = (" << no_bx << ","
                      << no_by << "," << no_bz << ")" << std::endl;
            return(-4);
        }
        if (rev_map[rev_map_index] != -1) {
            std::cerr << "Duplicate mapping found for anger merge block index on detector "
                      << idx << std::endl;
            std::cerr << "Assuming block size of (x,y,z) = (" << no_bx << ","
                      << no_by << "," << no_bz << ")" << std::endl;
            return(-5);
        }
        rev_map[rev_map_index] = idx;
    }

    merge_func = MergeAngerLogicFunctor(bx, by, bz, no_bx, no_by, no_bz,
                                        rev_map);
    return(0);
}

int InteractionStream::add_merge_process(
        const std::string & map_name,
        double merge_time,
        const std::vector<std::string> & options)
{
    if (id_maps.count(map_name) == 0) {
        std::cerr << "Unknown id map type: " << map_name << std::endl;
        return(-1);
    }
    std::string merge_type = "max";
    MergeF merge_func;
    const std::vector<int> id_map = id_maps[map_name];
    if (!options.empty()) {
        merge_type = options[0];
    }
    if (merge_type == "max") {
        merge_func = MergeMaxFunctor();
    } else if (merge_type == "first") {
        merge_func = MergeFirstFunctor();
    } else if (merge_type == "anger") {
        if (make_anger_func(options, merge_func) < 0) {
            std::cerr << "unable to create anger merge: " << std::endl;
            return(-2);
        }
    } else {
        std::cerr << "Unknown merge type: " << merge_type << std::endl;
        return(-1);
    }

    merge_processes.push_back(new MergeProcT(id_map, merge_time,
                                             get_time_func, get_id_func,
                                             merge_func));
    process_stream.add_process(merge_processes.back());
    return(0);
}

int InteractionStream::add_filter_process(const std::string & filter_name,
                                          double value)
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

int InteractionStream::add_blur_process(
        const std::string & name,
        double value,
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
                return(-1);
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
        sort_processes.push_back(new SortProcT(max_blur * 2,
                                               get_time_func));
        process_stream.add_process(sort_processes.back(), false);
        return(0);
    } else {
        std::cerr << "Unknown blur type: " << name << std::endl;
        return(-1);
    }
}

int InteractionStream::add_sort_process(
        const std::string & name,
        double value,
        const std::vector<std::string> & options,
        bool user_requested)
{
    if (name == "time") {
        sort_processes.push_back(new SortProcT(value, get_time_func));
        process_stream.add_process(sort_processes.back(), user_requested);
        return(0);
    } else {
        std::cerr << "Unknown sort type: " << name << std::endl;
        return(-1);
    }
}

int InteractionStream::add_coinc_process(const std::string & name, double value,
                                         const std::vector<std::string> & options)
{
    bool paralyzable = false;
    bool reject_multiples = true;
    bool combinatorial_pair_all_multiples = false;
    TimeT window_offset = 0;
    bool look_for_offset = false;
    if (name == "window") {
    } else if (name == "delay") {
        look_for_offset = true;
    } else {
        std::cerr << "Unknown coinc type: " << name << std::endl;
        return(-2);
    }
    size_t option_start = 0;
    if (look_for_offset) {
        option_start = 1;
        if (options.empty()) {
            std::cerr << "no delay offset specified: " << std::endl;
            return(-1);
        }
        std::stringstream ss(options[0]);
        if ((ss >> window_offset).fail()) {
            std::cerr << "invalid coinc delay offset: " << ss.str()
            << std::endl;
            return(-1);
        }


    }
    for (size_t ii = option_start; ii < options.size(); ii++) {
        const std::string & option = options[ii];
        if (option == "keep_multiples") {
            reject_multiples = false;
        } else if (option == "paralyzable") {
            paralyzable = true;
        } else if (option == "pair_all") {
            combinatorial_pair_all_multiples = true;
        } else {
            std::cerr << "unrecognized coinc option: " << option
            << std::endl;
            return(-1);
        }
    }
    auto tag_event_func = [](EventT & e, long id) {e.coinc_id = id;};
    coinc_processes.push_back(new CoincProcT(value, get_time_func,
                                             tag_event_func, reject_multiples,
                                             combinatorial_pair_all_multiples,
                                             paralyzable, window_offset));
    process_stream.add_parallel_out_process(coinc_processes.back());
    return(0);
}

int InteractionStream::add_deadtime_process(
        const std::string & map_name,
        double deadtime,
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
            std::cerr << "unrecognized deadtime option: " << option
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
                                               get_time_func, get_id_func,
                                               paralyzable));
    process_stream.add_process(deadtime_processes.back());
    return(0);
}
