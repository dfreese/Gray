#include <Output/Output.h>
#include <iomanip>
#include <sstream>
#include <map>
#include <Physics/Interaction.h>

using namespace std;

bool Output::write_header_flag = true;

void Output::DisableHeader() {
    write_header_flag = false;
}

Output::Output()
{
}

Output::~Output()
{
    log_file.close();
}

void Output::Close() {
    log_file.close();
}

int Output::MakeLogWord(int interaction, int color, int scatter, int det_mat,
                        int src_id)
{
    return (((interaction << 28) & (0xF0000000)) |
            ((color << 24)       & (0x0F000000)) |
            ((scatter << 20)     & (0x00F00000)) |
            ((det_mat << 12)     & (0x000FF000)) |
            (src_id              & (0x00000FFF)));
}

void Output::SetFormat(Format format) {
    this->format = format;
}

void Output::LogHits(const vector<Interaction>::const_iterator & begin,
                     const vector<Interaction>::const_iterator & end)
{
    for (auto iter = begin; iter != end; ++iter) {
        LogInteraction(*iter);
    }
}

void Output::LogSingles(const vector<Interaction>::const_iterator & begin,
                        const vector<Interaction>::const_iterator & end)
{
    for (auto iter = begin; iter != end; ++iter) {
        const auto & interact = *iter;
        if (!interact.dropped) {
            LogInteraction(interact);
        }
    }
}

void Output::LogCoinc(const vector<Interaction>::const_iterator & begin,
                      const vector<Interaction>::const_iterator & end,
                      bool pair_all)
{

    multimap<int, vector<Interaction>::const_iterator> ids;
    for (auto iter = begin; iter != end; ++iter) {
        const auto & interact = *iter;
        if (interact.coinc_id >= 0) {
            ids.insert(ids.end(), {interact.coinc_id, iter});
        }
    }

    for (auto iter = ids.begin(); iter != ids.end(); /* use iter_back to adv*/)
    {
        auto iter_back = std::next(iter);
        for (; iter_back != ids.end(); ++iter_back)
        {
            if ((*iter).first != (*iter_back).first) {
                break;
            }
        }
        if (pair_all) {
            for (auto key_iter0 = iter; key_iter0 != iter_back; key_iter0++) {
                for (auto key_iter1 = std::next(key_iter0);
                     key_iter1 != iter_back; key_iter1++)
                {
                    const Interaction & inter0 = *(*key_iter0).second;
                    const Interaction & inter1 = *(*key_iter1).second;
                    if (inter0.det_id <= inter1.det_id) {
                        LogInteraction(inter0);
                        LogInteraction(inter1);
                    } else {
                        LogInteraction(inter1);
                        LogInteraction(inter0);
                    }
                }
            }
        } else {
            for (auto key_iter = iter; key_iter != iter_back; key_iter++) {
                LogInteraction(*(*key_iter).second);
            }
        }
        iter = iter_back;
    }
}

void Output::LogInteractions(const vector<Interaction> & interactions) {
    if (format == VARIABLE_ASCII) {
        for (const auto & interact: interactions) {
            write_variable_ascii(interact, log_file, var_format_write_flags);
        }
    } else if (format == VARIABLE_BINARY) {
        for (const auto & interact: interactions) {
            write_variable_binary(interact, log_file, var_format_write_flags);
        }
    } else if (format == FULL_BINARY) {
        vector<GrayBinaryStandard> events(interactions.size());
        for (size_t ii = 0; ii < events.size(); ii++) {
            GrayBinaryStandard & b = events[ii];
            const Interaction & interact = interactions[ii];
            b.i = interact.decay_id;
            b.time = interact.time;
            b.energy = interact.energy;
            b.x = (float) interact.pos.x;
            b.y = (float) interact.pos.y;
            b.z = (float) interact.pos.z;
            b.det_id = interact.det_id;
            b.log = MakeLogWord(interact.type, interact.color,
                                interact.scatter_compton_phantom, interact.mat_id,
                                interact.src_id);
        }
        log_file.write(reinterpret_cast<char*>(events.data()),
                       events.size() * sizeof(GrayBinaryStandard));
    } else if (format == NO_POS_BINARY) {
        vector<GrayBinaryNoPosition> events(interactions.size());
        for (size_t ii = 0; ii < events.size(); ii++) {
            GrayBinaryNoPosition & b = events[ii];
            const Interaction & interact = interactions[ii];
            b.i = interact.decay_id;
            b.time = interact.time;
            b.energy = interact.energy;
            b.det_id = interact.det_id;
            b.log = MakeLogWord(interact.type, interact.color,
                                interact.scatter_compton_phantom, interact.mat_id,
                                interact.src_id);
        }
        log_file.write(reinterpret_cast<char*>(events.data()),
                       events.size() * sizeof(GrayBinaryNoPosition));
    } else if (format == FULL_ASCII) {
        for (const auto & interact: interactions) {
            char str[256];
            log_file << " " << interact.type << " ";
            log_file << interact.decay_id;
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
}

void Output::LogInteraction(const Interaction & interact) {
    switch (format) {
        case VARIABLE_ASCII: {
            write_variable_ascii(interact, log_file, var_format_write_flags);
        } break;
        case VARIABLE_BINARY: {
            write_variable_binary(interact, log_file, var_format_write_flags);
        } break;
        case FULL_BINARY: {
            GrayBinaryStandard b;
            b.i = interact.decay_id;
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
        } break;
        case NO_POS_BINARY: {
            GrayBinaryNoPosition b;
            b.i = interact.decay_id;
            b.time = interact.time;
            b.energy = interact.energy;
            b.det_id = interact.det_id;
            b.log = MakeLogWord(interact.type, interact.color,
                                interact.scatter_compton_phantom, interact.mat_id,
                                interact.src_id);
            log_file.write(reinterpret_cast<char*>(&b), sizeof(b));
        } break;
        case FULL_ASCII: {
            char str[256];
            log_file << " " << interact.type << " ";
            log_file << interact.decay_id;
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
        } break;
        default: {

        } break;
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

    if (write_header_flag) {
        if (format == VARIABLE_ASCII) {
            write_header(log_file, false);
            write_write_flags(var_format_write_flags, log_file, false);
        } else if (format == VARIABLE_BINARY) {
            write_header(log_file, true);
            write_write_flags(var_format_write_flags, log_file, true);
        }
    }

    return(true);
}

int Output::ParseFormat(const std::string & identifier, Output::Format & fmt) {
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

void Output::write_flags_stats(const WriteFlags & flags, int & no_fields,
                               int & no_active)
{
    no_fields = 0;
    no_active = 0;
    no_fields++; if (flags.time) no_active++;
    no_fields++; if (flags.decay_id) no_active++;
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
    no_fields++; if (flags.coinc_id) no_active++;
}

int Output::event_size(const WriteFlags & flags) {
    int event_size = 0;
    if (flags.time) event_size += sizeof(Interaction::time);
    if (flags.decay_id) event_size += sizeof(Interaction::decay_id);
    if (flags.color) event_size += sizeof(Interaction::color);
    if (flags.type) event_size += sizeof(Interaction::type);
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
    if (flags.coinc_id) {
        event_size += sizeof(Interaction::coinc_id);
    }
    return(event_size);
}

Output::WriteOffsets Output::event_offsets(const WriteFlags & flags) {
    WriteOffsets offsets;
    int event_size = 0;
    if (flags.time) {
        offsets.time = event_size;
        event_size += sizeof(Interaction::time);
    }
    if (flags.decay_id) {
        offsets.decay_id = event_size;
        event_size += sizeof(Interaction::decay_id);
    }
    if (flags.color) {
        offsets.color = event_size;
        event_size += sizeof(Interaction::color);
    }
    if (flags.type) {
        offsets.type = event_size;
        event_size += sizeof(Interaction::type);
    }
    if (flags.pos) {
        offsets.pos = event_size;
        event_size += sizeof(Interaction::pos.x);
        event_size += sizeof(Interaction::pos.y);
        event_size += sizeof(Interaction::pos.z);
    }
    if (flags.energy) {
        offsets.energy = event_size;
        event_size += sizeof(Interaction::energy);
    }
    if (flags.det_id) {
        offsets.det_id = event_size;
        event_size += sizeof(Interaction::det_id);
    }
    if (flags.src_id) {
        offsets.src_id = event_size;
        event_size += sizeof(Interaction::src_id);
    }
    if (flags.mat_id) {
        offsets.mat_id = event_size;
        event_size += sizeof(Interaction::mat_id);
    }
    if (flags.scatter_compton_phantom) {
        offsets.scatter_compton_phantom = event_size;
        event_size += sizeof(Interaction::scatter_compton_phantom);
    }
    if (flags.scatter_compton_detector) {
        offsets.scatter_compton_detector = event_size;
        event_size += sizeof(Interaction::scatter_compton_detector);
    }
    if (flags.scatter_rayleigh_phantom) {
        offsets.scatter_rayleigh_phantom = event_size;
        event_size += sizeof(Interaction::scatter_rayleigh_phantom);
    }
    if (flags.scatter_rayleigh_detector) {
        offsets.scatter_rayleigh_detector = event_size;
        event_size += sizeof(Interaction::scatter_rayleigh_detector);
    }
    if (flags.xray_flouresence) {
        offsets.xray_flouresence = event_size;
        event_size += sizeof(Interaction::xray_flouresence);
    }
    if (flags.coinc_id) {
        offsets.coinc_id = event_size;
        event_size += sizeof(Interaction::coinc_id);
    }
    return(offsets);
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

        int decay_id = flags.decay_id;
        output.write(reinterpret_cast<char*>(&decay_id), sizeof(decay_id));

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

        int coinc_id = flags.coinc_id;
        output.write(reinterpret_cast<char*>(&coinc_id), sizeof(coinc_id));
    } else {
        output << "no_fields " << no_fields << "\n" << "no_active "
        << no_active << "\n";
        output << "time " << static_cast<int>(flags.time) << "\n"
        << "decay_id " << static_cast<int>(flags.decay_id) << "\n"
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
        << static_cast<int>(flags.xray_flouresence) << "\n"
        << "coinc_id "
        << static_cast<int>(flags.coinc_id) << "\n";
    }

    if (output.fail()) {
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
    line_ss >> flags.decay_id;
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
    line_ss >> flags.coinc_id;
    return(!line_ss.fail());
}

bool Output::write_variable_ascii(const Interaction & inter,
                                  std::ostream & output,
                                  const WriteFlags & flags)
{
    if (flags.time) {
        output << " " << std::resetiosflags(std::ios::floatfield)
        << std::scientific << std::setprecision(23) << inter.time;
    }
    if (flags.decay_id) {
        output << " " << std::setw(9) << inter.decay_id;
    }
    if (flags.color) {
        output << " " << std::setw(3) << inter.color;
    }
    if (flags.type) {
        output << " " << std::setw(3) << inter.type;
    }
    if (flags.pos) {
        output << resetiosflags(ios::floatfield)
        << " " << scientific << setw(13) << setprecision(6) << inter.pos.x
        << " " << scientific << setw(13) << setprecision(6) << inter.pos.y
        << " " << scientific << setw(13) << setprecision(6) << inter.pos.z;
    }
    if (flags.energy) {
        output << resetiosflags(ios::floatfield)
        << " " << scientific << setw(12) << setprecision(6) << inter.energy;
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
    if (flags.coinc_id) {
        output << " " << std::setw(9) << inter.coinc_id;
    }
    output << "\n";
    if (output.fail()) {
        return(false);
    } else {
        return(true);
    }
}

bool Output::write_variable_binary(const Interaction & inter,
                                   std::ostream & output,
                                   const WriteFlags & flags)
{
    if (flags.time) {
        output.write(reinterpret_cast<const char*>(&inter.time),
                     sizeof(inter.time));
    }
    if (flags.decay_id) {
        output.write(reinterpret_cast<const char*>(&inter.decay_id),
                     sizeof(inter.decay_id));
    }
    if (flags.color) {
        output.write(reinterpret_cast<const char*>(&inter.color),
                     sizeof(inter.color));
    }
    if (flags.type) {
        output.write(reinterpret_cast<const char*>(&inter.type),
                     sizeof(inter.type));
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
    if (flags.coinc_id) {
        output.write(reinterpret_cast<const char*>(&inter.coinc_id),
                     sizeof(inter.coinc_id));
    }

    if (output.fail()) {
        return(false);
    } else {
        return(true);
    }
}

void Output::SetVariableOutputMask(const WriteFlags & flags) {
    var_format_write_flags = flags;
}
