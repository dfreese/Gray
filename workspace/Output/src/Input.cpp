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
    if (format == Output::VARIABLE_ASCII) {
        return(read_variable_ascii(interact, log_file, var_format_write_flags));
    } else if (format == Output::VARIABLE_BINARY) {
        return(read_variable_binary(interact, log_file,
                                    var_format_write_flags));
    } else if (format == Output::FULL_BINARY) {
        GrayBinaryStandard b;
        if (!log_file.read(reinterpret_cast<char*>(&b), sizeof(b))) {
            return(false);
        }
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
        return(true);
    } else if (format == Output::NO_POS_BINARY) {
        GrayBinaryNoPosition b;
        if (!log_file.read(reinterpret_cast<char*>(&b), sizeof(b))) {
            return(false);
        }
        interact.decay_id = b.i;
        interact.time = b.time;
        interact.energy = b.energy;
        interact.det_id = b.det_id;
        parse_log_word(b.log, interact.type, interact.color,
                       interact.scatter_compton_phantom, interact.mat_id,
                       interact.src_id);
        return(true);
    } else if (format == Output::FULL_ASCII) {
        string line;
        if (!getline(log_file, line)) {
            return(false);
        }
        stringstream ss(line);
        bool fail = (ss >> interact.type).fail();
        fail |= (ss >> interact.decay_id).fail();
        fail |= (ss >> interact.color).fail();
        fail |= (ss >> interact.time).fail();
        fail |= (ss >> interact.energy).fail();
        fail |= (ss >> interact.pos.x).fail();
        fail |= (ss >> interact.pos.y).fail();
        fail |= (ss >> interact.pos.z).fail();
        fail |= (ss >> interact.src_id).fail();
        fail |= (ss >> interact.scatter_compton_phantom).fail();
        fail |= (ss >> interact.mat_id).fail();
        fail |= (ss >> interact.det_id).fail();
        return(!fail);
    } else {
        throw std::runtime_error("Unsupported input format type");
    }
}

bool read_interactions(std::vector<Interaction> & interactions) {
    // TODO: read all of the file formats in chunks
    throw runtime_error("read_interactions not implemented");
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
        success &= read_header(log_file, false, var_format_version);
        success &= read_write_flags(var_format_write_flags, log_file, false);
    } else if (format == Output::VARIABLE_BINARY) {
        success &= read_header(log_file, true, var_format_version);
        success &= read_write_flags(var_format_write_flags, log_file, true);
    }

    return(success);
}

bool Input::read_header(std::istream & input, bool binary, int & version)
{
    if (binary) {
        int magic_number;
        input.read(reinterpret_cast<char *>(&magic_number),
                   sizeof(magic_number));
        if (magic_number != Output::header_start_magic_number) {
            return(false);
        }
        input.read(reinterpret_cast<char *>(&version), sizeof(version));
    } else {
        string line;
        getline(input, line);
        stringstream line_ss(line);
        line_ss >> line; // dump the "gray_output_version" text
        if ((line_ss >> version).fail()) {
            return(false);
        }
    }
    if (input.fail()) {
        return(false);
    } else {
        return(true);
    }
}

bool Input::read_write_flags(Output::WriteFlags & flags, std::istream & input,
                             bool binary)
{
    int read_no_fields;
    int read_no_active;

    if (binary) {
        input.read(reinterpret_cast<char*>(&read_no_fields),
                   sizeof(read_no_fields));
        input.read(reinterpret_cast<char*>(&read_no_active),
                   sizeof(read_no_active));

        int per_event_size;
        input.read(reinterpret_cast<char*>(&per_event_size),
                   sizeof(per_event_size));

        int read_val;
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.time = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.decay_id = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.color = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.type = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.pos = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.energy = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.det_id = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.src_id = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.mat_id = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.scatter_compton_phantom = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.scatter_compton_detector = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.scatter_rayleigh_phantom = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.scatter_rayleigh_detector = static_cast<bool>(read_val);
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.xray_flouresence = static_cast<bool>(read_val);

        int expected_per_event_size = Output::event_size(flags);
        if (expected_per_event_size != per_event_size) {
            return(false);
        }

    } else {
        string line;
        stringstream ss;
        string value_name;

        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> read_no_fields;

        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> read_no_active;

        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.time;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.decay_id;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.color;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.type;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.pos;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.energy;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.det_id;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.src_id;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.mat_id;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.scatter_compton_phantom;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.scatter_compton_detector;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.scatter_rayleigh_phantom;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.scatter_rayleigh_detector;
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.xray_flouresence;
    }

    int no_fields;
    int no_active;
    Output::write_flags_stats(flags, no_fields, no_active);
    if ((read_no_fields != no_fields) || (read_no_active != no_active)) {
        return(false);
    }

    if (input.fail()) {
        return(false);
    } else {
        return(true);
    }
}

bool Input::read_variable_binary(Interaction & inter, std::istream & input,
                                 const Output::WriteFlags & flags)
{
    if (flags.time) {
        input.read(reinterpret_cast<char*>(&inter.time),
                   sizeof(inter.time));
    }
    if (flags.decay_id) {
        input.read(reinterpret_cast<char*>(&inter.decay_id),
                   sizeof(inter.decay_id));
    }
    if (flags.color) {
        input.read(reinterpret_cast<char*>(&inter.color),
                   sizeof(inter.color));
    }
    if (flags.type) {
        input.read(reinterpret_cast<char*>(&inter.type),
                   sizeof(inter.type));
    }
    if (flags.pos) {
        input.read(reinterpret_cast<char*>(&inter.pos.x),
                   sizeof(inter.pos.x));
        input.read(reinterpret_cast<char*>(&inter.pos.y),
                   sizeof(inter.pos.y));
        input.read(reinterpret_cast<char*>(&inter.pos.z),
                   sizeof(inter.pos.z));
    }
    if (flags.energy) {
        input.read(reinterpret_cast<char*>(&inter.energy),
                   sizeof(inter.energy));
    }
    if (flags.det_id) {
        input.read(reinterpret_cast<char*>(&inter.det_id),
                   sizeof(inter.det_id));
    }
    if (flags.src_id) {
        input.read(reinterpret_cast<char*>(&inter.src_id),
                   sizeof(inter.src_id));
    }
    if (flags.mat_id) {
        input.read(reinterpret_cast<char*>(&inter.mat_id),
                   sizeof(inter.mat_id));
    }
    if (flags.scatter_compton_phantom) {
        input.read(reinterpret_cast<char*>(&inter.scatter_compton_phantom),
                   sizeof(inter.scatter_compton_phantom));
    }
    if (flags.scatter_compton_detector) {
        input.read(reinterpret_cast<char*>(&inter.scatter_compton_detector),
                   sizeof(inter.scatter_compton_detector));
    }
    if (flags.scatter_rayleigh_phantom) {
        input.read(reinterpret_cast<char*>(&inter.scatter_rayleigh_phantom),
                   sizeof(inter.scatter_rayleigh_phantom));
    }
    if (flags.scatter_rayleigh_detector) {
        input.read(reinterpret_cast<char*>(&inter.scatter_rayleigh_detector),
                   sizeof(inter.scatter_rayleigh_detector));
    }
    if (flags.xray_flouresence) {
        input.read(reinterpret_cast<char*>(&inter.xray_flouresence),
                   sizeof(inter.xray_flouresence));
    }
    if (flags.coinc_id) {
        input.read(reinterpret_cast<char*>(&inter.coinc_id),
                   sizeof(inter.coinc_id));
    }
    return(input.good());
}


bool Input::read_variable_ascii(Interaction & inter, std::istream & input,
                                 const Output::WriteFlags & flags)
{
    string line;
    if (!getline(input, line)) {
        return(false);
    }
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
    return(!line_ss.fail());
}

void Input::set_variable_mask(const Output::WriteFlags & flags) {
    var_format_write_flags = flags;
}

Output::WriteFlags Input::get_write_flags() const {
    return(var_format_write_flags);
}
