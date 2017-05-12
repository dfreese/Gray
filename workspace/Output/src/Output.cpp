#include <Output/Output.h>
#include <iomanip>
#include <sstream>
#include <Physics/Interaction.h>

using namespace std;

Output::Output(const std::string & logfile, Format fmt) :
    format(fmt)
{
    if (logfile != "") {
        SetLogfile(logfile);
    }
}

Output::~Output()
{
    log_file.close();
}

int Output::MakeLogWord(Interaction::INTER_TYPE interaction, int color,
                        bool scatter, int det_mat, int src_id)
{
    return (((interaction << 28) & (0x70000000)) |
            ((color << 26)       & (0x0C000000)) |
            ((scatter << 24)     & (0x01000000)) |
            ((det_mat << 12)     & (0x00FFF000)) |
            (src_id              & (0x00000FFF)));
}

void Output::SetFormat(Format format) {
    this->format = format;
}

void Output::LogInteraction(const Interaction & interact) {
    if (format == VARIABLE_ASCII) {
        write_interaction(interact, log_file, var_format_write_flags, false);
    } else if (format == VARIABLE_BINARY) {
        write_interaction(interact, log_file, var_format_write_flags, true);
    } else if (format == FULL_BINARY) {
        GrayBinaryStandard b;
        b.i = interact.id;
        b.time = interact.time;
        b.energy = interact.energy;
        b.x = (float) interact.pos.x;
        b.y = (float) interact.pos.y;
        b.z = (float) interact.pos.z;
        b.det_id = interact.det_id;
        b.log = MakeLogWord(interact.type, interact.color,
                            interact.scatter_compton_phantom, interact.mat_id,
                            interact.src_id);
        log_file.write(reinterpret_cast<char*>(&b), sizeof(b));
    } else if (format == NO_POS_BINARY) {
        GrayBinaryNoPosition b;
        b.i = interact.id;
        b.time = interact.time;
        b.energy = interact.energy;
        b.det_id = interact.det_id;
        b.log = MakeLogWord(interact.type, interact.color,
                            interact.scatter_compton_phantom, interact.mat_id,
                            interact.src_id);
        log_file.write(reinterpret_cast<char*>(&b), sizeof(b));
    } else if (format == FULL_ASCII) {
        char str[256];
        log_file << " " << interact.type << " ";
        log_file << interact.id;
        log_file << " " << interact.color << " ";
        sprintf(str,"%23.16e ", interact.time);
        log_file << str;
        sprintf(str,"%12.6e ", interact.energy);
        log_file << str;
        sprintf(str,"%15.8e %15.8e %15.8e %2d ", (float) interact.pos.x,
                (float) interact.pos.y,
                (float) interact.pos.z,
                interact.src_id);
        log_file << str;
        if (interact.scatter_compton_phantom) {
            log_file << " 1 ";
        } else {
            log_file << " 0 ";
        }
        sprintf(str,"%2d ", interact.mat_id);
        log_file << str;
        sprintf(str,"%3d ", interact.det_id);
        log_file << str;
        log_file << "\n";
    }
}

bool Output::SetLogfile(const std::string & name)
{
    log_file.open(name.c_str(), ios::out);
    if (log_file.fail()) {
        cerr << "ERROR: cannot open ";
        cerr << name;
        cerr << " log file.\n";
        return false;
    }

    if (format == VARIABLE_ASCII) {
        write_header(log_file, false);
        write_write_flags(var_format_write_flags, log_file, false);
    } else if (format == VARIABLE_BINARY) {
        write_header(log_file, true);
        write_write_flags(var_format_write_flags, log_file, true);
    }

    return(true);
}

int Output::GetFormat(const std::string & identifier, Output::Format & fmt) {
    if (identifier == "var_ascii") {
        fmt = VARIABLE_ASCII;
    } else if (identifier == "var_binary") {
        fmt = VARIABLE_BINARY;
    } else if (identifier == "full_ascii") {
        fmt = FULL_ASCII;
    } else if (identifier == "full_binary") {
        fmt = FULL_BINARY;
    } else if (identifier == "no_pos_binary") {
        fmt = NO_POS_BINARY;
    } else {
        stringstream ss(identifier);
        int type;
        if ((ss >> type).fail()) {
            return(-1);
        }
        if (type == FULL_ASCII) {
            fmt = FULL_ASCII;
        } else if (type == FULL_BINARY) {
            fmt = FULL_BINARY;
        } else if (type == NO_POS_BINARY) {
            fmt = NO_POS_BINARY;
        } else {
            return(-2);
        }
    }
    return(0);
}

int Output::header_start_magic_number = 0xFFFB;
int Output::output_version_number = 1;
bool Output::write_header(std::ostream & output, bool binary) {
    if (binary) {
        output.write(reinterpret_cast<char *>(&header_start_magic_number),
                     sizeof(header_start_magic_number));
        output.write(reinterpret_cast<char *>(&output_version_number),
                     sizeof(output_version_number));
    } else {
        output << "gray_output_version " << output_version_number << "\n";
    }

    if (output.fail()) {
        return(false);
    } else {
        return(true);
    }
}

bool Output::read_header(std::istream & input, bool & binary, int & version)
{
    auto init_pos = input.tellg();
    int magic_number;
    input.read(reinterpret_cast<char *>(&magic_number), sizeof(magic_number));
    if (magic_number == header_start_magic_number) {
        binary = true;
        input.read(reinterpret_cast<char *>(&version), sizeof(version));
    } else {
        binary = false;
        input.seekg(init_pos);
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

void Output::write_flags_stats(const WriteFlags & flags, int & no_fields,
                               int & no_active)
{
    no_fields = 0;
    no_active = 0;
    no_fields++; if (flags.time) no_active++;
    no_fields++; if (flags.id) no_active++;
    no_fields++; if (flags.color) no_active++;
    no_fields++; if (flags.type) no_active++;
    no_fields++; if (flags.pos) no_active++;
    no_fields++; if (flags.energy) no_active++;
    no_fields++; if (flags.det_id) no_active++;
    no_fields++; if (flags.src_id) no_active++;
    no_fields++; if (flags.mat_id) no_active++;
    no_fields++; if (flags.scatter_compton_phantom) no_active++;
    no_fields++; if (flags.scatter_compton_detector) no_active++;
    no_fields++; if (flags.scatter_rayleigh_phantom) no_active++;
    no_fields++; if (flags.scatter_rayleigh_detector) no_active++;
    no_fields++; if (flags.xray_flouresence) no_active++;
}

int Output::event_size(const WriteFlags & flags) {
    int event_size = 0;
    if (flags.time) event_size += sizeof(Interaction::time);
    if (flags.id) event_size += sizeof(Interaction::id);
    if (flags.color) event_size += sizeof(int);
    if (flags.type) event_size += sizeof(int);
    if (flags.pos) {
        event_size += sizeof(Interaction::pos.x);
        event_size += sizeof(Interaction::pos.y);
        event_size += sizeof(Interaction::pos.z);
    }
    if (flags.energy) event_size += sizeof(Interaction::energy);
    if (flags.det_id) event_size += sizeof(Interaction::det_id);
    if (flags.src_id) event_size += sizeof(Interaction::src_id);
    if (flags.mat_id) event_size += sizeof(Interaction::mat_id);
    if (flags.scatter_compton_phantom) {
        event_size += sizeof(Interaction::scatter_compton_phantom);
    }
    if (flags.scatter_compton_detector) {
        event_size += sizeof(Interaction::scatter_compton_detector);
    }
    if (flags.scatter_rayleigh_phantom) {
        event_size += sizeof(Interaction::scatter_rayleigh_phantom);
    }
    if (flags.scatter_rayleigh_detector) {
        event_size += sizeof(Interaction::scatter_rayleigh_detector);
    }
    if (flags.xray_flouresence) {
        event_size += sizeof(Interaction::xray_flouresence);
    }
    return(event_size);
}

bool Output::write_write_flags(const WriteFlags & flags,
                               std::ostream & output, bool binary)
{
    int no_fields;
    int no_active;
    write_flags_stats(flags, no_fields, no_active);
    if (binary) {
        output.write(reinterpret_cast<char*>(&no_fields), sizeof(no_fields));

        output.write(reinterpret_cast<char*>(&no_active), sizeof(no_active));

        int per_event_size = event_size(flags);
        output.write(reinterpret_cast<char*>(&per_event_size),
                     sizeof(per_event_size));

        int time = flags.time;
        output.write(reinterpret_cast<char*>(&time), sizeof(time));

        int id = flags.id;
        output.write(reinterpret_cast<char*>(&id), sizeof(id));

        int color = flags.color;
        output.write(reinterpret_cast<char*>(&color), sizeof(color));

        int type = flags.type;
        output.write(reinterpret_cast<char*>(&type), sizeof(type));

        int pos = flags.pos;
        output.write(reinterpret_cast<char*>(&pos), sizeof(pos));

        int energy = flags.energy;
        output.write(reinterpret_cast<char*>(&energy), sizeof(energy));

        int det_id = flags.det_id;
        output.write(reinterpret_cast<char*>(&det_id), sizeof(det_id));

        int src_id = flags.src_id;
        output.write(reinterpret_cast<char*>(&src_id), sizeof(src_id));

        int mat_id = flags.mat_id;
        output.write(reinterpret_cast<char*>(&mat_id), sizeof(mat_id));

        int scatter_compton_phantom = flags.scatter_compton_phantom;
        output.write(reinterpret_cast<char*>(&scatter_compton_phantom),
                     sizeof(scatter_compton_phantom));

        int scatter_compton_detector = flags.scatter_compton_detector;
        output.write(reinterpret_cast<char*>(&scatter_compton_detector),
                     sizeof(scatter_compton_detector));

        int scatter_rayleigh_phantom = flags.scatter_rayleigh_phantom;
        output.write(reinterpret_cast<char*>(&scatter_rayleigh_phantom),
                     sizeof(scatter_rayleigh_phantom));

        int scatter_rayleigh_detector = flags.scatter_rayleigh_detector;
        output.write(reinterpret_cast<char*>(&scatter_rayleigh_detector),
                     sizeof(scatter_rayleigh_detector));

        int xray_flouresence = flags.xray_flouresence;
        output.write(reinterpret_cast<char*>(&xray_flouresence),
                     sizeof(xray_flouresence));
    } else {
        output << "no_fields " << no_fields << "\n" << "no_active "
        << no_active << "\n";
        output << "time " << static_cast<int>(flags.time) << "\n"
        << "id " << static_cast<int>(flags.id) << "\n"
        << "color " << static_cast<int>(flags.color) << "\n"
        << "type " << static_cast<int>(flags.type) << "\n"
        << "pos " << static_cast<int>(flags.pos) << "\n"
        << "energy " << static_cast<int>(flags.energy) << "\n"
        << "det_id " << static_cast<int>(flags.det_id) << "\n"
        << "src_id " << static_cast<int>(flags.src_id) << "\n"
        << "mat_id " << static_cast<int>(flags.mat_id) << "\n"
        << "scatter_compton_phantom "
        << static_cast<int>(flags.scatter_compton_phantom) << "\n"
        << "scatter_compton_detector "
        << static_cast<int>(flags.scatter_compton_detector) << "\n"
        << "scatter_rayleigh_phantom "
        << static_cast<int>(flags.scatter_rayleigh_phantom) << "\n"
        << "scatter_rayleigh_detector "
        << static_cast<int>(flags.scatter_rayleigh_detector) << "\n"
        << "xray_flouresence "
        << static_cast<int>(flags.xray_flouresence) << "\n";
    }

    if (output.fail()) {
        return(false);
    } else {
        return(true);
    }
}

bool Output::read_write_flags(WriteFlags & flags, std::istream & input,
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
        flags.id = static_cast<bool>(read_val);
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

        int expected_per_event_size = event_size(flags);
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
        ss >> value_name;  ss >> flags.id;
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
    write_flags_stats(flags, no_fields, no_active);
    if ((read_no_fields != no_fields) || (read_no_active != no_active)) {
        return(false);
    }

    if (input.fail()) {
        return(false);
    } else {
        return(true);
    }
}

/*!
 * Turn a string of ones and zeros into write flags
 */
bool Output::parse_write_flags_mask(WriteFlags & flags,
                                    const std::string & mask)
{
    stringstream line_ss(mask);
    line_ss >> flags.time;
    line_ss >> flags.id;
    line_ss >> flags.color;
    line_ss >> flags.type;
    line_ss >> flags.pos;
    line_ss >> flags.energy;
    line_ss >> flags.det_id;
    line_ss >> flags.src_id;
    line_ss >> flags.mat_id;
    line_ss >> flags.scatter_compton_phantom;
    line_ss >> flags.scatter_compton_detector;
    line_ss >> flags.scatter_rayleigh_phantom;
    line_ss >> flags.scatter_rayleigh_detector;
    line_ss >> flags.xray_flouresence;
    return(!line_ss.fail());
}

bool Output::write_interaction(const Interaction & inter,
                               std::ostream & output,
                               const WriteFlags & flags,
                               bool binary)
{
    if (binary) {
        if (flags.time) {
            output.write(reinterpret_cast<const char*>(&inter.time),
                         sizeof(inter.time));
        }
        if (flags.id) {
            output.write(reinterpret_cast<const char*>(&inter.id),
                         sizeof(inter.id));
        }
        if (flags.color) {
            int color = inter.color;
            output.write(reinterpret_cast<const char*>(&color),
                         sizeof(color));
        }
        if (flags.type) {
            int type = inter.type;
            output.write(reinterpret_cast<const char*>(&type), sizeof(type));
        }
        if (flags.pos) {
            output.write(reinterpret_cast<const char*>(&inter.pos.x),
                         sizeof(inter.pos.x));
            output.write(reinterpret_cast<const char*>(&inter.pos.y),
                         sizeof(inter.pos.y));
            output.write(reinterpret_cast<const char*>(&inter.pos.z),
                         sizeof(inter.pos.z));
        }
        if (flags.energy) {
            output.write(reinterpret_cast<const char*>(&inter.energy),
                         sizeof(inter.energy));
        }
        if (flags.det_id) {
            output.write(reinterpret_cast<const char*>(&inter.det_id),
                         sizeof(inter.det_id));
        }
        if (flags.src_id) {
            output.write(reinterpret_cast<const char*>(&inter.src_id),
                         sizeof(inter.src_id));
        }
        if (flags.mat_id) {
            output.write(reinterpret_cast<const char*>(&inter.mat_id),
                         sizeof(inter.mat_id));
        }
        if (flags.scatter_compton_phantom) {
            output.write(reinterpret_cast<const char*>(&inter.scatter_compton_phantom),
                         sizeof(inter.scatter_compton_phantom));
        }
        if (flags.scatter_compton_detector) {
            output.write(reinterpret_cast<const char*>(&inter.scatter_compton_detector),
                         sizeof(inter.scatter_compton_detector));
        }
        if (flags.scatter_rayleigh_phantom) {
            output.write(reinterpret_cast<const char*>(&inter.scatter_rayleigh_phantom),
                         sizeof(inter.scatter_rayleigh_phantom));
        }
        if (flags.scatter_rayleigh_detector) {
            output.write(reinterpret_cast<const char*>(&inter.scatter_rayleigh_detector),
                         sizeof(inter.scatter_rayleigh_detector));
        }
        if (flags.xray_flouresence) {
            output.write(reinterpret_cast<const char*>(&inter.xray_flouresence),
                         sizeof(inter.xray_flouresence));
        }
    } else {
        if (flags.time) {
            output << " " << std::resetiosflags(std::ios::floatfield)
            << std::scientific << std::setprecision(23) << inter.time;
        }
        if (flags.id) {
            output << " " << std::setw(9) << inter.id;
        }
        if (flags.color) {
            output << " " << std::setw(3) << static_cast<int>(inter.color);
        }
        if (flags.type) {
            output << " " << std::setw(3) << static_cast<int>(inter.type);
        }
        if (flags.pos) {
            output << " " << std::resetiosflags(std::ios::floatfield)
            << std::scientific << std::setprecision(6) << inter.pos.x
            << " " << inter.pos.y << " " << inter.pos.z;
        }
        if (flags.energy) {
            output << " " << std::resetiosflags(std::ios::floatfield)
            << std::scientific << std::setprecision(6) << inter.energy;
        }
        if (flags.det_id) {
            output << " " << std::setw(5) << inter.det_id;
        }
        if (flags.src_id) {
            output << " " << std::setw(5) << inter.src_id;
        }
        if (flags.mat_id) {
            output << " " << std::setw(5) << inter.mat_id;
        }
        if (flags.scatter_compton_phantom) {
            output << " " << std::setw(5) << inter.scatter_compton_phantom;
        }
        if (flags.scatter_compton_detector) {
            output << " " << std::setw(5) << inter.scatter_compton_detector;
        }
        if (flags.scatter_rayleigh_phantom) {
            output << " " << std::setw(5) << inter.scatter_rayleigh_phantom;
        }
        if (flags.scatter_rayleigh_detector) {
            output << " " << std::setw(5) << inter.scatter_rayleigh_detector;
        }
        if (flags.xray_flouresence) {
            output << " " << std::setw(3) << inter.xray_flouresence;
        }
        output << "\n";
    }

    if (output.fail()) {
        return(false);
    } else {
        return(true);
    }
}

bool Output::read_interaction(Interaction & inter, std::istream & input,
                              const WriteFlags & flags, bool binary)
{
    if (binary) {
        if (flags.time) {
            input.read(reinterpret_cast<char*>(&inter.time),
                       sizeof(inter.time));
        }
        if (flags.id) {
            input.read(reinterpret_cast<char*>(&inter.id),
                       sizeof(inter.id));
        }
        if (flags.color) {
            int value;
            input.read(reinterpret_cast<char*>(&value),
                       sizeof(value));
            inter.color = static_cast<Photon::Color>(value);
        }
        if (flags.type) {
            int value;
            input.read(reinterpret_cast<char*>(&value),
                       sizeof(value));
            inter.type = static_cast<Interaction::INTER_TYPE>(value);
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
    } else {
        string line;
        getline(input, line);
        stringstream line_ss(line);
        if (flags.time) {
            line_ss >> inter.time;
        }
        if (flags.id) {
            line_ss >> inter.id;
        }
        if (flags.color) {
            int color;
            line_ss >> color;
            inter.color = static_cast<Photon::Color>(color);
        }
        if (flags.type) {
            int type;
            line_ss >> type;
            inter.type = static_cast<Interaction::INTER_TYPE>(type);
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
    }
    return(input.good());
}

void Output::SetVariableOutputMask(const WriteFlags & flags) {
    var_format_write_flags = flags;
}
