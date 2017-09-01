#include <Output/Input.h>
#include <sstream>
#include <Physics/Interaction.h>

using namespace std;

Input::Input()
{
}

Input::~Input()
{
    log_file.close();
}

void Input::set_format(Output::Format format) {
    this->format = format;
}


void Input::parse_log_word(int log, int & interaction, int & color,
                           int & scatter, int & det_mat, int & src_id)
{
    interaction = ((log & 0xF0000000) >> 28);
    color = ((log & 0x0F000000) >> 24);
    scatter = ((log & 0x00F00000) >> 20);
    det_mat = ((log & 0x000FF000) >> 12);
    src_id = ((log & 0x00000FFF) >> 0);
}

bool Input::read_interaction(Interaction & interact) {
    std::vector<Interaction> interactions;
    bool val = read_interactions(interactions, 1);
    if (val) {
        interact = interactions.front();
        return(true);
    } else {
        return(false);
    }
}

/*!
 * returns true if any events could be read.  false if no events could be read.
 * Guarantees no iterators are invalidated if false is returned.
 */
bool Input::read_interactions(std::vector<Interaction> & interactions,
                              size_t no_interactions)
{
    switch (format) {
        case Output::VARIABLE_ASCII:
            return(read_variables_ascii(interactions, no_interactions, log_file,
                                        var_format_write_flags));
            break;
        case Output::VARIABLE_BINARY:
            return(read_variables_binary(interactions, no_interactions,
                                         log_file, var_format_write_flags));
            break;
        case Output::FULL_BINARY:
            return (read_full_binary(interactions, no_interactions, log_file));
            break;
        case Output::NO_POS_BINARY:
            return (read_no_pos_binary(interactions, no_interactions, log_file));
            break;
        case Output::FULL_ASCII:
            return (read_full_ascii(interactions, no_interactions, log_file));
            break;
        default:
            throw std::runtime_error("Unsupported input format type");
            break;
    }
}

bool Input::set_logfile(const std::string & name)
{
    log_file.open(name.c_str());
    if (log_file.fail()) {
        cerr << "ERROR: cannot open ";
        cerr << name;
        cerr << " log file.\n";
        return false;
    }

    bool success = true;
    if (format == Output::VARIABLE_ASCII) {
        success &= read_header_ascii(log_file, var_format_version);
        success &= read_write_flags_ascii(var_format_write_flags, log_file);
    } else if (format == Output::VARIABLE_BINARY) {
        success &= read_header_binary(log_file, var_format_version);
        success &= read_write_flags_binary(var_format_write_flags, log_file);
    }
    return(success);
}

bool Input::read_header_binary(std::istream & input, int & version)
{
    vector<int> input_vals(2);
    if (!input.read(reinterpret_cast<char*>(input_vals.data()),
                    input_vals.size() * sizeof(int)))
    {
        return(false);
    }
    int magic_number = input_vals[0];
    if (magic_number != Output::header_start_magic_number) {
        return(false);
    }
    version = input_vals[1];
    return(true);
}

bool Input::read_header_ascii(std::istream & input, int & version)
{
    string line;
    getline(input, line);
    stringstream line_ss(line);
    line_ss >> line; // dump the "gray_output_version" text
    line_ss >> version;
    if (input.fail() || line_ss.fail()) {
        return(false);
    } else {
        return(true);
    }
}


bool Input::read_write_flags_binary(Output::WriteFlags & flags,
                                    std::istream & input)
{
    vector<int> input_vals(18);
    if (!input.read(reinterpret_cast<char*>(input_vals.data()),
                    input_vals.size() * sizeof(int)))
    {
        return(false);
    }
    int read_no_fields = input_vals[0];
    int read_no_active = input_vals[1];
    int per_event_size = input_vals[2];
    flags.time = static_cast<bool>(input_vals[3]);
    flags.decay_id = static_cast<bool>(input_vals[4]);
    flags.color = static_cast<bool>(input_vals[5]);
    flags.type = static_cast<bool>(input_vals[6]);
    flags.pos = static_cast<bool>(input_vals[7]);
    flags.energy = static_cast<bool>(input_vals[8]);
    flags.det_id = static_cast<bool>(input_vals[9]);
    flags.src_id = static_cast<bool>(input_vals[10]);
    flags.mat_id = static_cast<bool>(input_vals[11]);
    flags.scatter_compton_phantom = static_cast<bool>(input_vals[12]);
    flags.scatter_compton_detector = static_cast<bool>(input_vals[13]);
    flags.scatter_rayleigh_phantom = static_cast<bool>(input_vals[14]);
    flags.scatter_rayleigh_detector = static_cast<bool>(input_vals[15]);
    flags.xray_flouresence = static_cast<bool>(input_vals[16]);
    flags.coinc_id = static_cast<bool>(input_vals[17]);

    int expected_per_event_size = Output::event_size(flags);
    if (expected_per_event_size != per_event_size) {
        return(false);
    }

    int no_fields;
    int no_active;
    Output::write_flags_stats(flags, no_fields, no_active);
    if ((read_no_fields != no_fields) || (read_no_active != no_active)) {
        return(false);
    }
    return(true);
}

bool Input::read_write_flags_ascii(Output::WriteFlags & flags,
                                   std::istream & input)
{
    vector<int> input_vals(17);
    vector<std::string> input_names(17);

    for (size_t ii = 0; ii < input_vals.size(); ii++) {
        string line;
        if (!getline(input, line)) {
            return(false);
        }
        stringstream ss(line);
        ss >> input_names[ii];
        ss >> input_vals[ii];
        if (ss.fail()) {
            return(false);
        }
    }
    int read_no_fields = input_vals[0];
    int read_no_active = input_vals[1];
    flags.time = static_cast<bool>(input_vals[2]);
    flags.decay_id = static_cast<bool>(input_vals[3]);
    flags.color = static_cast<bool>(input_vals[4]);
    flags.type = static_cast<bool>(input_vals[5]);
    flags.pos = static_cast<bool>(input_vals[6]);
    flags.energy = static_cast<bool>(input_vals[7]);
    flags.det_id = static_cast<bool>(input_vals[8]);
    flags.src_id = static_cast<bool>(input_vals[9]);
    flags.mat_id = static_cast<bool>(input_vals[10]);
    flags.scatter_compton_phantom = static_cast<bool>(input_vals[11]);
    flags.scatter_compton_detector = static_cast<bool>(input_vals[12]);
    flags.scatter_rayleigh_phantom = static_cast<bool>(input_vals[13]);
    flags.scatter_rayleigh_detector = static_cast<bool>(input_vals[14]);
    flags.xray_flouresence = static_cast<bool>(input_vals[15]);
    flags.coinc_id = static_cast<bool>(input_vals[16]);

    int no_fields;
    int no_active;
    Output::write_flags_stats(flags, no_fields, no_active);
    if ((read_no_fields != no_fields) || (read_no_active != no_active)) {
        return(false);
    }
    return(true);
}

bool Input::read_variables_binary(std::vector<Interaction> & interactions,
                                  size_t no_interactions,
                                  std::istream & input,
                                  const Output::WriteFlags & flags)
{
    size_t event_size = Output::event_size(flags);
    Output::WriteOffsets offsets = Output::event_offsets(flags);
    vector<char> read_buf(event_size * no_interactions);
    input.read(read_buf.data(), read_buf.size());
    size_t no_events = no_interactions;
    if (input.fail()) {
        if (input.bad()) {
            return(false);
        }
        // Handle the case where we didn't read as much as we requested.
        size_t no_events = input.gcount() / event_size;
        // Bail if we were somehow at the end of the file
        if (no_events == 0) {
            return(false);
        }
        // Or if the bytes we read doesn't match with what's required for
        // an event.
        if ((input.gcount() % event_size) != 0) {
            return(false);
        }
        read_buf.resize(input.gcount());
    }
    interactions.reserve(interactions.size() + no_events);


    for (size_t offset = 0; offset < read_buf.size(); offset += event_size) {
        char * event_ptr = &read_buf[offset];
        interactions.emplace_back();
        Interaction & inter = interactions.back();
        if (flags.time) {
            inter.time = *reinterpret_cast<double*>(event_ptr + offsets.time);
        }
        if (flags.decay_id) {
            inter.decay_id = *reinterpret_cast<int*>(event_ptr +
                                                     offsets.decay_id);
        }
        if (flags.color) {
            inter.color = *reinterpret_cast<int*>(event_ptr + offsets.color);
        }
        if (flags.type) {
            inter.type = *reinterpret_cast<int*>(event_ptr + offsets.type);
        }
        if (flags.pos) {
            inter.pos.x = *reinterpret_cast<double*>(event_ptr + offsets.pos);
            inter.pos.y = *reinterpret_cast<double*>(event_ptr + offsets.pos +
                                                     sizeof(inter.pos.x));
            inter.pos.z = *reinterpret_cast<double*>(event_ptr + offsets.pos +
                                                     sizeof(inter.pos.x) +
                                                     sizeof(inter.pos.y));
        }
        if (flags.energy) {
            inter.energy = *reinterpret_cast<double*>(event_ptr +
                                                      offsets.energy);
        }
        if (flags.det_id) {
            inter.det_id = *reinterpret_cast<int*>(event_ptr + offsets.det_id);
        }
        if (flags.src_id) {
            inter.src_id = *reinterpret_cast<int*>(event_ptr + offsets.src_id);
        }
        if (flags.mat_id) {
            inter.mat_id = *reinterpret_cast<int*>(event_ptr + offsets.mat_id);
        }
        if (flags.scatter_compton_phantom) {
            inter.scatter_compton_phantom = *reinterpret_cast<int*>(
                    event_ptr + offsets.scatter_compton_phantom);
        }
        if (flags.scatter_compton_detector) {
            inter.scatter_compton_detector = *reinterpret_cast<int*>(
                    event_ptr + offsets.scatter_compton_detector);
        }
        if (flags.scatter_rayleigh_phantom) {
            inter.scatter_rayleigh_phantom = *reinterpret_cast<int*>(
                    event_ptr + offsets.scatter_rayleigh_phantom);
        }
        if (flags.scatter_rayleigh_detector) {
            inter.scatter_rayleigh_detector = *reinterpret_cast<int*>(
                    event_ptr + offsets.scatter_rayleigh_detector);
        }
        if (flags.xray_flouresence) {
            inter.xray_flouresence = *reinterpret_cast<int*>(
                    event_ptr + offsets.xray_flouresence);
        }
        if (flags.coinc_id) {
            inter.coinc_id = *reinterpret_cast<int*>(event_ptr +
                                                     offsets.coinc_id);
        }
    }
    return(true);
}

bool Input::read_variables_ascii(std::vector<Interaction> & interactions,
                                 size_t no_interactions,
                                 std::istream & input,
                                 const Output::WriteFlags & flags)
{
    interactions.reserve(interactions.size() + no_interactions);
    string line;
    for (size_t ii = 0; (ii < no_interactions) & (!getline(input, line).fail()); ii++) {
        Interaction inter;
        stringstream line_ss(line);
        if (flags.time) {
            line_ss >> inter.time;
        }
        if (flags.decay_id) {
            line_ss >> inter.decay_id;
        }
        if (flags.color) {
            line_ss >> inter.color;
        }
        if (flags.type) {
            line_ss >> inter.type;
        }
        if (flags.pos) {
            line_ss >> inter.pos.x;
            line_ss >> inter.pos.y;
            line_ss >> inter.pos.z;
        }
        if (flags.energy) {
            line_ss >> inter.energy;
        }
        if (flags.det_id) {
            line_ss >> inter.det_id;
        }
        if (flags.src_id) {
            line_ss >> inter.src_id;
        }
        if (flags.mat_id) {
            line_ss >> inter.mat_id;
        }
        if (flags.scatter_compton_phantom) {
            line_ss >> inter.scatter_compton_phantom;
        }
        if (flags.scatter_compton_detector) {
            line_ss >> inter.scatter_compton_detector;
        }
        if (flags.scatter_rayleigh_phantom) {
            line_ss >> inter.scatter_rayleigh_phantom;
        }
        if (flags.scatter_rayleigh_detector) {
            line_ss >> inter.scatter_rayleigh_detector;
        }
        if (flags.xray_flouresence) {
            line_ss >> inter.xray_flouresence;
        }
        if (flags.coinc_id) {
            line_ss >> inter.coinc_id;
        }
        if (line_ss.fail()) {
            break;
        }
        interactions.push_back(inter);
    }
    if (interactions.empty()) {
        return(false);
    } else {
        return(true);
    }
}

bool Input::read_no_pos_binary(std::vector<Interaction> & interactions,
                               size_t no_interactions, std::istream & input)
{
    constexpr size_t event_size = sizeof(GrayBinaryNoPosition);
    vector<GrayBinaryNoPosition> read_buf(no_interactions);
    input.read(reinterpret_cast<char *>(read_buf.data()), read_buf.size() * event_size);
    size_t no_events = no_interactions;
    if (input.fail()) {
        if (input.bad()) {
            return(false);
        }
        // Handle the case where we didn't read as much as we requested.
        size_t no_events = input.gcount() / event_size;
        // Bail if we were somehow at the end of the file
        if (no_events == 0) {
            return(false);
        }
        // Or if the bytes we read doesn't match with what's required for
        // an event.
        if ((input.gcount() % event_size) != 0) {
            return(false);
        }
        read_buf.resize(no_events);
    }
    interactions.reserve(interactions.size() + no_events);


    for (auto & b: read_buf) {
        interactions.emplace_back();
        Interaction & interact = interactions.back();
        interact.decay_id = b.i;
        interact.time = b.time;
        interact.energy = b.energy;
        interact.det_id = b.det_id;
        parse_log_word(b.log, interact.type, interact.color,
                       interact.scatter_compton_phantom, interact.mat_id,
                       interact.src_id);
    }
    return(true);
}

bool Input::read_full_binary(std::vector<Interaction> & interactions,
                             size_t no_interactions, std::istream & input)
{
    constexpr size_t event_size = sizeof(GrayBinaryStandard);
    vector<GrayBinaryStandard> read_buf(no_interactions);
    input.read(reinterpret_cast<char *>(read_buf.data()), read_buf.size() * event_size);
    size_t no_events = no_interactions;
    if (input.fail()) {
        if (input.bad()) {
            return(false);
        }
        // Handle the case where we didn't read as much as we requested.
        size_t no_events = input.gcount() / event_size;
        // Bail if we were somehow at the end of the file
        if (no_events == 0) {
            return(false);
        }
        // Or if the bytes we read doesn't match with what's required for
        // an event.
        if ((input.gcount() % event_size) != 0) {
            return(false);
        }
        read_buf.resize(no_events);
    }
    interactions.reserve(interactions.size() + no_events);


    for (auto & b: read_buf) {
        interactions.emplace_back();
        Interaction & interact = interactions.back();
        interact.decay_id = b.i;
        interact.time = b.time;
        interact.energy = b.energy;
        interact.pos.x = b.x;
        interact.pos.y = b.y;
        interact.pos.z = b.z;
        interact.det_id = b.det_id;
        parse_log_word(b.log, interact.type, interact.color,
                       interact.scatter_compton_phantom, interact.mat_id,
                       interact.src_id);
    }
    return(true);
}

bool Input::read_full_ascii(std::vector<Interaction> & interactions,
                            size_t no_interactions, std::istream & input)
{
    std::vector<Interaction> read_buf(no_interactions);
    size_t no_events = 0;
    for (; no_events < no_interactions; ++no_events) {
        string line;
        if (!getline(input, line)) {
            break;
        }
        stringstream ss(line);
        Interaction & interact = read_buf[no_events];
        ss >> interact.type;
        ss >> interact.decay_id;
        ss >> interact.color;
        ss >> interact.time;
        ss >> interact.energy;
        ss >> interact.pos.x;
        ss >> interact.pos.y;
        ss >> interact.pos.z;
        ss >> interact.src_id;
        ss >> interact.scatter_compton_phantom;
        ss >> interact.mat_id;
        ss >> interact.det_id;
        if (ss.fail()) {
            break;
        }
    }
    if (no_events == 0) {
        return(false);
    }

    interactions.insert(interactions.end(), read_buf.begin(), read_buf.end());
    return(true);
}

void Input::set_variable_mask(const Output::WriteFlags & flags) {
    var_format_write_flags = flags;
}

Output::WriteFlags Input::get_write_flags() const {
    return(var_format_write_flags);
}
