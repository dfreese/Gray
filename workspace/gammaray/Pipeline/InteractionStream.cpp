#include <Pipeline/InteractionStream.h>
#include <cmath>
#include <exception>
#include <Random/Random.h>

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
    for (const auto& proc: processes) {
        dropped += proc->no_dropped();
    }
    return(dropped);
}

long InteractionStream::no_merged() const {
    long merged = 0;
    for (const auto& proc: processes) {
        if (dynamic_cast<MergeProcT*>(proc.get())) {
            merged += proc->no_dropped();
        }
    }
    return(merged);
}

long InteractionStream::no_filtered() const {
    long filtered = 0;
    for (const auto& proc: processes) {
        if (dynamic_cast<FilterProcT*>(proc.get())) {
            filtered += proc->no_dropped();
        }
    }
    return(filtered);
}

long InteractionStream::no_deadtimed() const {
    long count = 0;
    for (const auto& proc: processes) {
        if (dynamic_cast<DeadtimeT*>(proc.get())) {
            count += proc->no_dropped();
        }
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
    os << "merged: " << s.no_merged() << "\n";
    os << "filtered: " << s.no_filtered() << "\n";
    os << "deadtimed: " << s.no_deadtimed() << "\n";
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

void InteractionStream::add_process(std::unique_ptr<ProcT> process,
                                    bool proc_print_info)
{
    processes.push_back(std::move(process));
    print_info.push_back(proc_print_info);
    process_ready_distance.push_back(0);
}

struct InteractionStream::MergeFirstFunctor {
    void operator() (EventT & e0, EventT & e1) {
        Interaction::MergeStats(e0, e1);
        e0.energy = e0.energy + e1.energy;
        e1.dropped = true;
    }
};

struct InteractionStream::MergeMaxFunctor {
    void operator() (EventT & e0, EventT & e1) {
        if (e0.energy < e1.energy) {
            Interaction::MergeStats(e1, e0);
            e1.energy = e0.energy + e1.energy;
            e0.dropped = true;
        } else {
            Interaction::MergeStats(e0, e1);
            e0.energy = e0.energy + e1.energy;
            e1.dropped = true;
        }
    }
};

struct InteractionStream::MergeAngerLogicFunctor {
    MergeAngerLogicFunctor(const std::vector<int> & base,
                           const std::vector<int> & bx,
                           const std::vector<int> & by,
                           const std::vector<int> & bz) :
        base(base),
        bx(bx),
        by(by),
        bz(bz),
        no_blk(*std::max_element(base.begin(), base.end()) + 1),
        no_bx(*std::max_element(bx.begin(), bx.end()) + 1),
        no_by(*std::max_element(by.begin(), by.end()) + 1),
        no_bz(*std::max_element(bz.begin(), bz.end()) + 1),
        reverse_map(create_reverse_map())
    {
    }

    std::vector<int> create_reverse_map() {
        const int total = static_cast<int>(base.size());
        const int implied_total = no_blk * no_bx * no_by * no_bz;
        if (total != implied_total) {
            std::stringstream err;
            err << total << " detectors specified, but anger logic mapping of "
                << no_blk << " blocks with a size of (x,y,z) = (" << no_bx
                << "," << no_by << "," << no_bz << ")" << " implies "
                << implied_total << " detectors.";
            throw std::runtime_error(err.str());

        }
        std::vector<int> reverse_map(total, -1);

        for (int idx = 0; idx < total; idx++) {
            int rev_idx = index(base[idx], bx[idx], by[idx], bz[idx]);
            if ((rev_idx < 0) || (rev_idx >= total)) {
                std::stringstream err;
                err << "Block index mapping is not consistent with block size "
                    << "(x,y,z) = (" << no_bx << "," << no_by << "," << no_bz
                    << ")" << " at detector " << idx;
                throw std::runtime_error(err.str());
            }
            if (reverse_map[rev_idx] != -1) {
                std::stringstream err;
                err << "Duplicate mapping found for anger merge with block"
                    << "size (x,y,z) = (" << no_bx << "," << no_by << ","
                    << no_bz << ")" << " at detector " << idx;
                throw std::runtime_error(err.str());
            }
            reverse_map[rev_idx] = idx;
        }
        return (reverse_map);
    }

    int index(int blk, int bx, int by, int bz) {
        return (((blk * no_bz + bz) * no_by + by) * no_bx + bx);
    }

    void operator() (EventT & e0, EventT & e1) {
        const float energy_result = e0.energy + e1.energy;
        // Base is inherently the same for both detectors inherently by being
        // matched in merge.
        const int blk = base[e0.det_id];
        const int row0 = bx[e0.det_id];
        const int row1 = bx[e1.det_id];
        const int col0 = by[e0.det_id];
        const int col1 = by[e1.det_id];
        const int lay0 = bz[e0.det_id];
        const int lay1 = bz[e1.det_id];
        const int row_result = static_cast<int>(std::round(
                static_cast<float>(row0) * (e0.energy / energy_result) +
                static_cast<float>(row1) * (e1.energy / energy_result)));
        const int col_result = static_cast<int>(std::round(
                static_cast<float>(col0) * (e0.energy / energy_result) +
                static_cast<float>(col1) * (e1.energy / energy_result)));
        const int lay_result = static_cast<int>(std::round(
                static_cast<float>(lay0) * (e0.energy / energy_result) +
                static_cast<float>(lay1) * (e1.energy / energy_result)));

        const int rev_idx = index(blk, row_result, col_result, lay_result);
        const int id_result = reverse_map[rev_idx];
        if (e0.energy < e1.energy) {
            Interaction::MergeStats(e1, e0);
            e1.det_id = id_result;
            e1.energy = energy_result;
            e0.dropped = true;
        } else {
            Interaction::MergeStats(e0, e1);
            e0.det_id = id_result;
            e0.energy = energy_result;
            e1.dropped = true;
        }
    }

    const std::vector<int> base;
    const std::vector<int> bx;
    const std::vector<int> by;
    const std::vector<int> bz;
    const int no_blk;
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

    const double value;
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

    const double value;
};


struct InteractionStream::BlurEnergyFunctor {
    BlurEnergyFunctor(double fwhm_percent) :
        value(fwhm_percent)
    {
    }

    void operator() (EventT & event) {
        event.energy = Random::GaussianEnergyBlur(event.energy, value);
    }

    const double value;
};


struct InteractionStream::BlurEnergyReferencedFunctor {
    BlurEnergyReferencedFunctor(double fwhm_percent, double ref_energy) :
        value(fwhm_percent),
        ref(ref_energy)
    {
    }

    void operator() (EventT & event) {
        event.energy = Random::GaussianEnergyBlurInverseSqrt(event.energy, value, ref);
    }

    const double value;
    const double ref;
};



struct InteractionStream::BlurTimeFunctor {
    BlurTimeFunctor(double fwhm_time, double max_blur) :
        value(fwhm_time),
        max(max_blur)
    {
    }

    void operator() (EventT & event) {
        event.time = Random::GaussianBlurTimeTrunc(event.time, value, max);
    }

    const double value;
    const double max;
};

int InteractionStream::make_anger_func(
        const std::string & map_name,
        const std::vector<std::string> & anger_opts,
        MergeF & merge_func)
{
    if (anger_opts.size() != 4) {
        std::cerr << "Error: anger merge requires 3 block mapping names"
                  << std::endl;
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

    try {
        merge_func = MergeAngerLogicFunctor(base, bx, by, bz);
    } catch (const std::runtime_error e) {
        std::cerr << e.what() << std::endl;
        return (-4);
    }
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
    add_process(std::unique_ptr<ProcT>(new MergeProcT(id_map, merge_time,
                                       get_time_func, get_id_func,
                                       merge_func)), true);
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
    add_process(std::unique_ptr<ProcT>(new FilterProcT(filt_func)), true);
    return(0);
}

int InteractionStream::add_blur_process(
        const std::string & name,
        double value,
        const std::vector<std::string> & options)
{
    if (name == "energy") {
        BlurF eblur;
        if (options.empty()) {
            eblur = BlurEnergyFunctor(value);
        } else if (options[0] == "at") {
            double ref_energy;
            std::stringstream ss(options[1]);
            if ((ss >> ref_energy).fail()) {
                std::cerr << "invalid reference energy: " << options[1]
                << std::endl;
                return(-1);
            }
            eblur = BlurEnergyReferencedFunctor(value, ref_energy);
        } else {
            std::cerr << "unrecognized blur option: " << options[0]
            << std::endl;
            return(-1);
        }
        add_process(std::unique_ptr<ProcT>(new BlurProcT(eblur)), true);
        return(0);
    } else if (name == "time") {
        // Allow the value to be 3 FWHM on either side of the current event
        // TODO: allow this to be set by options.
        const TimeT max_blur = 3 * value;
        auto tblur = BlurTimeFunctor(value, max_blur);
        add_process(std::unique_ptr<ProcT>(new BlurProcT(tblur)), true);

        // Sort the stream afterwards, based on the capped blur
        add_process(std::unique_ptr<ProcT>(new SortProcT(max_blur * 2,
                                                         get_time_func)),
                    false);
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
        add_process(std::unique_ptr<ProcT>(new SortProcT(value, get_time_func)),
                    user_requested);
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
    coinc_processes.emplace_back(new CoincProcT(value, get_time_func,
                                                tag_event_func,
                                                reject_multiples,
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
    add_process(std::unique_ptr<ProcT>(new DeadtimeT(id_map, deadtime,
                                                     get_time_func, get_id_func,
                                                     paralyzable)), true);
    return(0);
}

InteractionStream::EventIter InteractionStream::begin() {
    return(input_events.begin());
}

InteractionStream::EventIter InteractionStream::end() {
    return(input_events.end());
}

InteractionStream::EventIter InteractionStream::hits_begin() {
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

InteractionStream::EventIter InteractionStream::hits_end() {
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

InteractionStream::EventIter InteractionStream::singles_begin() {
    if (singles_stopped) {
        return(singles_ready);
    } else {
        return(begin());
    }
}

InteractionStream::EventIter InteractionStream::singles_end() {
    if (singles_stopped) {
        return(end());
    } else {
        return(singles_ready);
    }
}

InteractionStream::EventIter InteractionStream::coinc_begin() {
    return(begin());
}

InteractionStream::EventIter InteractionStream::coinc_end() {
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
