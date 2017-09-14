#include <Pipeline/InteractionStream.h>

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

size_t InteractionStream::no_processes() const {
    return(processes.size());
}

size_t InteractionStream::no_coinc_processes() const {
    return(coinc_processes.size());
}

long InteractionStream::no_events() const {
    if (!processes.empty()) {
        return (processes.front()->no_events());
    } else if (!coinc_processes.empty()) {
        return (coinc_processes.front()->no_events());
    } else {
        return (0);
    }
}

long InteractionStream::no_kept() const {
    if (!processes.empty()) {
        return (processes.back()->no_kept());
    } else {
        return (0);
    }
}

long InteractionStream::no_dropped() const {
    long dropped = 0;
    for (auto proc: processes) {
        dropped += proc->no_dropped();
    }
    return(dropped);
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

long InteractionStream::no_deadtimed() const {
    long count = 0;
    for (auto & proc: deadtime_processes) {
        count += proc->no_dropped();
    }
    return(count);
}

std::ostream & operator << (std::ostream & os, const InteractionStream & s) {
    os << "events: " << s.no_events() << "\n"
       << "kept: " << s.no_kept() << "\n"
       << "dropped: " << s.no_dropped() << "\n"
       << "drop per level: ";
    for (size_t idx = 0; idx < s.processes.size(); idx++) {
        if (s.print_info[idx]) {
            os << " " << s.processes[idx]->no_dropped() << ",";
        }
    }
    os << "\n";
    if (!s.merge_processes.empty()) {
        os << "merged: " << s.no_merged() << "\n";
    }
    if (!s.filter_processes.empty()) {
        os << "filtered: " << s.no_filtered() << "\n";
    }
    if (!s.deadtime_processes.empty()) {
        os << "deadtimed: " << s.no_deadtimed() << "\n";
    }
    if (!s.coinc_processes.empty()) {
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

void InteractionStream::add_process(ProcT * process, bool proc_print_info) {
    processes.push_back(process);
    print_info.push_back(proc_print_info);
    process_ready_distance.push_back(0);
}

struct InteractionStream::MergeFirstFunctor {
    void operator() (EventT & e0, const EventT & e1) {
        Interaction::MergeStats(e0, e1);
        e0.energy = e0.energy + e1.energy;
    }
};

struct InteractionStream::MergeMaxFunctor {
    void operator() (EventT & e0, const EventT & e1) {
        e0.det_id = e0.energy > e1.energy ? e0.det_id:e1.det_id;
        Interaction::MergeStats(e0, e1);
        e0.decay_id = e0.energy > e1.energy ? e0.decay_id:e1.decay_id;
        e0.color = e0.energy > e1.energy ? e0.color:e1.color;
        e0.energy = e0.energy + e1.energy;
    }
};

struct InteractionStream::MergeAngerLogicFunctor {
    MergeAngerLogicFunctor(const std::vector<int> & base_map,
                           const std::vector<int> & bx_map,
                           const std::vector<int> & by_map,
                           const std::vector<int> & bz_map,
                           int nx, int ny, int nz,
                           const std::vector<int> & rev_map) :
        base(base_map),
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
        // Base is inherently the same for both detectors inherently by being
        // matched in merge.
        const int blk = base[e0.det_id];
        const int row0 = bx[e0.det_id];
        const int row1 = bx[e1.det_id];
        const int col0 = by[e0.det_id];
        const int col1 = by[e1.det_id];
        const int lay0 = bz[e0.det_id];
        const int lay1 = bz[e1.det_id];
        int row_result = static_cast<int>(
                static_cast<float>(row0) * (e0.energy / energy_result) +
                static_cast<float>(row1) * (e1.energy / energy_result));
        int col_result = static_cast<int>(
                static_cast<float>(col0) * (e0.energy / energy_result) +
                static_cast<float>(col1) * (e1.energy / energy_result));
        int lay_result = static_cast<int>(
                static_cast<float>(lay0) * (e0.energy / energy_result) +
                static_cast<float>(lay1) * (e1.energy / energy_result));

        int rev_idx = ((blk * no_bx + row_result) * no_by + col_result) * no_bz + lay_result;
        Interaction::MergeStats(e0, e1);
        e0.decay_id = e0.energy > e1.energy ? e0.decay_id:e1.decay_id;
        e0.color = e0.energy > e1.energy ? e0.color:e1.color;
        e0.energy = energy_result;
        e0.det_id = reverse_map[rev_idx];
    }

    const std::vector<int> base;
    const std::vector<int> bx;
    const std::vector<int> by;
    const std::vector<int> bz;
    const int no_bx;
    const int no_by;
    const int no_bz;
    const std::vector<int> reverse_map;
};


struct InteractionStream::FilterEnergyGateLowFunctor {
    FilterEnergyGateLowFunctor(double energy_val) :
        value(energy_val)
    {
    }

    bool operator() (EventT & event) {
        bool val = event.energy >= value;
        if (!val) {
            event.dropped = true;
        }
        return(val);
    }

    double value;
};


struct InteractionStream::FilterEnergyGateHighFunctor {
    FilterEnergyGateHighFunctor(double energy_val) :
    value(energy_val)
    {
    }

    bool operator() (EventT & event) {
        bool val = event.energy <= value;
        if (!val) {
            event.dropped = true;
        }
        return(val);
    }

    double value;
};


int InteractionStream::make_anger_func(
        const std::string & map_name,
        const std::vector<std::string> & anger_opts,
        MergeF & merge_func)
{
    if (anger_opts.size() != 4) {
        std::cerr << "Error: anger merge requires 3 block mapping names" << std::endl;
        return(-1);
    }
    std::vector<std::string> block_maps(3, "");
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
    const std::vector<int> & base = this->id_maps[map_name];
    const std::vector<int> & bx = this->id_maps[block_maps[0]];
    const std::vector<int> & by = this->id_maps[block_maps[1]];
    const std::vector<int> & bz = this->id_maps[block_maps[2]];
    const int no_bx = *std::max_element(bx.begin(), bx.end()) + 1;
    const int no_by = *std::max_element(by.begin(), by.end()) + 1;
    const int no_bz = *std::max_element(bz.begin(), bz.end()) + 1;
    const int total = static_cast<int>(base.size());

    std::vector<int> rev_map(total, -1);
    for (int idx = 0; idx < total; idx++) {
        int rev_map_index = ((base[idx] * no_bx + bx[idx]) * no_by + by[idx]) * no_bz + bz[idx];
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

    assert(std::count(rev_map.begin(), rev_map.end(), -1) == 0);

    merge_func = MergeAngerLogicFunctor(base, bx, by, bz, no_bx, no_by, no_bz,
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
        if (make_anger_func(map_name, options, merge_func) < 0) {
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

    add_process(merge_processes.back(), true);
    return(0);
}

int InteractionStream::add_filter_process(const std::string & filter_name,
                                          double value)
{
    FilterF filt_func;
    if (filter_name == "egate_low") {
        filt_func = FilterEnergyGateLowFunctor(value);
    } else if (filter_name == "egate_high") {
        filt_func = FilterEnergyGateHighFunctor(value);
    } else {
        std::cerr << "Unknown filter type: " << filter_name << std::endl;
        return(-1);
    }
    filter_processes.push_back(new FilterProcT(filt_func));
    add_process(filter_processes.back(), true);
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
            add_process(blur_processes.back(), true);
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
            add_process(blur_processes.back(), true);
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
        add_process(blur_processes.back(), true);

        // Sort the stream afterwards, based on the capped blur
        sort_processes.push_back(new SortProcT(max_blur * 2,
                                               get_time_func));
        add_process(sort_processes.back(), false);
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
        add_process(sort_processes.back(), user_requested);
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
        } else {
            std::cerr << "unrecognized coinc option: " << option
            << std::endl;
            return(-1);
        }
    }
    auto tag_event_func = [](EventT & e, long id) {e.coinc_id = id;};
    coinc_processes.push_back(new CoincProcT(value, get_time_func,
                                             tag_event_func, reject_multiples,
                                             paralyzable, window_offset));
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
    add_process(deadtime_processes.back(), true);
    return(0);
}

std::vector<Interaction>::iterator InteractionStream::begin() {
    return(input_events.begin());
}

std::vector<Interaction>::iterator InteractionStream::end() {
    return(input_events.end());
}

std::vector<Interaction>::iterator InteractionStream::hits_begin() {
    if (hits_stopped) {
        if (process_ready_distance.empty()) {
            return(begin());
        } else {
            return(begin() + process_ready_distance.front());
        }
    } else {
        return(begin());
    }
}

std::vector<Interaction>::iterator InteractionStream::hits_end() {
    if (hits_stopped) {
        return(end());
    } else {
        if (process_ready_distance.empty()) {
            return(begin());
        } else {
            return(begin() + process_ready_distance.front());
        }
    }
}

std::vector<Interaction>::iterator InteractionStream::singles_begin() {
    if (singles_stopped) {
        return(singles_ready);
    } else {
        return(begin());
    }
}

std::vector<Interaction>::iterator InteractionStream::singles_end() {
    if (singles_stopped) {
        return(end());
    } else {
        return(singles_ready);
    }
}


std::vector<Interaction>::iterator InteractionStream::coinc_begin() {
    return(begin());
}

std::vector<Interaction>::iterator InteractionStream::coinc_end() {

    if (coinc_stopped) {
        return(end());
    } else {
        return(coinc_ready);
    }
}

/*!
 * Only run the first process, which is always a sorting process in gray.  This
 * should not be called if initial_sort_window was not specified.
 */
void InteractionStream::process_hits() {
    singles_stopped = false;
    singles_ready = input_events.end();
    if (!processes.empty()) {
        const auto begin = input_events.begin() + process_ready_distance.front();
        singles_ready = processes.front()->process_events(begin, singles_ready);
        process_ready_distance.front() = std::distance(input_events.begin(), singles_ready);
    }
    min_coinc_ready_dist = std::distance(input_events.begin(), singles_ready);
}

void InteractionStream::process_singles() {
    singles_stopped = false;
    // We might have left some singles that we processed in the buffer from
    // previously, so start where we left off.
    singles_ready = input_events.end();
    for (size_t ii = 0; ii < processes.size(); ii++) {
        const auto begin = input_events.begin() + process_ready_distance[ii];
        singles_ready = processes[ii]->process_events(begin, singles_ready);
        process_ready_distance[ii] = std::distance(input_events.begin(), singles_ready);
    }
    min_coinc_ready_dist = std::distance(input_events.begin(), singles_ready);
}

void InteractionStream::process_coinc(size_t idx) {
    coinc_stopped = false;
    coinc_ready = coinc_processes[idx]->process_events(input_events.begin(),
                                                       singles_ready);
    min_coinc_ready_dist = std::min(min_coinc_ready_dist,
                                    std::distance(input_events.begin(), coinc_ready));
}

void InteractionStream::stop_hits() {
    hits_stopped = true;
    if (!processes.empty()) {
        processes.front()->stop(begin() + process_ready_distance.front(), end());
    }
}


void InteractionStream::stop_singles() {
    singles_stopped = true;
    for (size_t ii = 0; ii < process_ready_distance.size(); ii++) {
        processes[ii]->stop(begin() + process_ready_distance[ii], end());
    }
}

void InteractionStream::stop_coinc(size_t idx) {
    coinc_stopped = true;
    coinc_processes[idx]->stop(begin(), end());
}

void InteractionStream::clear_complete() {
    auto singles_dist = std::distance(input_events.begin(), singles_ready);
    input_events.erase(begin(), begin() + min_coinc_ready_dist);
    for (auto & dist: process_ready_distance) {
        dist -= min_coinc_ready_dist;
    }
    singles_ready = input_events.begin() + (singles_dist - min_coinc_ready_dist);
}
