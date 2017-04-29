#include <Physics/Interaction.h>
#include <math.h>
#include <Physics/GammaStats.h>
#include <Physics/NuclearDecay.h>
#include <Random/Random.h>
#include <stdlib.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;

Interaction::Interaction() :
    type(NO_INTERACTION),
    id(0),
    time(0),
    pos(0, 0, 0),
    energy(0),
    color(Photon::P_BLUE),
    src_id(0),
    mat_id(0),
    det_id(-1),
    scatter_compton_phantom(0),
    scatter_compton_detector(0),
    scatter_rayleigh_phantom(0),
    scatter_rayleigh_detector(0),
    xray_flouresence(0),
    sensitive_mat(false)
{
}

Interaction Interaction::NoInteraction() {
    return(Interaction());
}

Interaction Interaction::NoInteraction(const Photon & p,
                                       const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = NO_INTERACTION;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::Photoelectric(const Photon & p,
                                       const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = PHOTOELECTRIC;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::XrayEscape(const Photon & p, double deposit,
                                 const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = XRAY_ESCAPE;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = deposit;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::Compton(const Photon & p, double deposit,
                                 const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = COMPTON;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = deposit;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::Rayleigh(const Photon & p,
                                  const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = RAYLEIGH;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = 0;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::NuclearDecay(const class NuclearDecay &p,
                                      const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = NUCLEAR_DECAY;
    hit.id = p.GetDecayNumber();
    hit.time = p.GetTime();
    hit.pos = p.GetPosition();
    hit.energy = p.GetEnergy();
    hit.color = Photon::P_YELLOW;
    hit.src_id = p.GetSourceId();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = -1;
    hit.scatter_compton_phantom = 0;
    hit.scatter_compton_detector = 0;
    hit.scatter_rayleigh_phantom = 0;
    hit.scatter_rayleigh_detector = 0;
    hit.xray_flouresence = 0;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}


Interaction Interaction::ErrorTraceDepth(const Photon & p,
                                         const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = ERROR_TRACE_DEPTH;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::ErrorEmtpy(const Photon & p)
{
    Interaction hit;
    hit.type = ERROR_TRACE_DEPTH;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = -1;
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    hit.sensitive_mat = false;
    return(hit);
}

void Interaction::merge_interactions(Interaction & i0, const Interaction & i1) {
    i0.energy += i1.energy;
    i0.scatter_compton_phantom += i1.scatter_compton_phantom;
    i0.scatter_compton_detector += i1.scatter_compton_detector;
    i0.scatter_rayleigh_phantom += i1.scatter_rayleigh_phantom;
    i0.scatter_rayleigh_detector += i1.scatter_rayleigh_detector;
    i0.xray_flouresence += i1.xray_flouresence;
}


int Interaction::header_start_magic_number = 0xFFFB;
int Interaction::output_version_number = 1;
bool Interaction::write_header(std::ostream & output, bool binary) {
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

bool Interaction::read_header(std::istream & input, bool & binary,
                              int & version)
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

void Interaction::write_flags_stats(const WriteFlags & flags, int & no_fields,
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
    no_fields++; if (flags.sensitive_mat) no_active++;
}

int Interaction::event_size(const WriteFlags & flags) {
    int event_size = 0;
    if (flags.time) event_size += sizeof(Interaction::time);
    if (flags.id) event_size += sizeof(Interaction::id);
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
    if (flags.sensitive_mat) event_size += sizeof(Interaction::sensitive_mat);
    return(event_size);
}

bool Interaction::write_write_flags(const WriteFlags & flags,
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
        
        int sensitive_mat = flags.sensitive_mat;
        output.write(reinterpret_cast<char*>(&sensitive_mat),
                     sizeof(sensitive_mat));
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
               << static_cast<int>(flags.xray_flouresence) << "\n"
               << "sensitive_mat "
               << static_cast<int>(flags.sensitive_mat) << "\n";
    }

    if (output.fail()) {
        return(false);
    } else {
        return(true);
    }
}

bool Interaction::read_write_flags(WriteFlags & flags, std::istream & input,
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
        input.read(reinterpret_cast<char*>(&read_val), sizeof(read_val));
        flags.sensitive_mat = static_cast<bool>(read_val);

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
        getline(input, line); ss.clear(); ss << line;
        ss >> value_name;  ss >> flags.sensitive_mat;
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

bool Interaction::write_interaction(const Interaction & inter,
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
        if (flags.sensitive_mat) {
            output.write(reinterpret_cast<const char*>(&inter.sensitive_mat),
                         sizeof(inter.sensitive_mat));
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
        if (flags.sensitive_mat) {
            output << " " << std::setw(3)
                   << static_cast<int>(inter.sensitive_mat);
        }
        output << "\n";
    }

    if (output.fail()) {
        return(false);
    } else {
        return(true);
    }
}

bool Interaction::read_interaction(Interaction & inter, std::istream & input,
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
        if (flags.sensitive_mat) {
            input.read(reinterpret_cast<char*>(&inter.sensitive_mat),
                       sizeof(inter.sensitive_mat));
        }
    } else {
        string line;
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
        if (flags.sensitive_mat) {
            line_ss >> inter.sensitive_mat;
        }
    }

    if (input.fail()) {
        return(false);
    } else {
        return(true);
    }
}

Interaction::KleinNishina::KleinNishina() :
    // These energies were chosen, as they give less than 0.5% error from 0 to
    // 1.5MeV when linear interpolation is performed.
    energy_idx({
        0.0, 0.010, 0.030, 0.050, 0.100, 0.200, 0.300, 0.400, 0.500, 0.600,
        0.700, 0.900, 1.100, 1.300, 1.500})
{
    dsigma_max_val.resize(energy_idx.size());
    std::transform(energy_idx.begin(), energy_idx.end(),
                   dsigma_max_val.begin(), find_max);
}

/*!
 * Calculates dsigma / dtheta for the Klein-Nishina formula to be used in a
 * accept/reject monte carlo.  For this reason the constants at the front of the
 * formula have been dropped out, as they will be divided out eventually by the
 * max.
 * In this case h is P(E, theta) as seen here:
 * https://en.wikipedia.org/wiki/Klein–Nishina_formula
 *
 */
double Interaction::KleinNishina::dsigma(double theta, double energy_mev)
{
    double alpha = energy_mev / ENERGY_511;
    double cs = cos(theta);
    double ss = sin(theta);
    double h = 1. / (1. + alpha * (1. - cs));
    double sigma = ss * h * h * (h + 1./ h - ss * ss);
    return(sigma);
}

/*!
 * Use std::upper_bound to binary search the closest value above the given
 * energy and then linearly interpolate the actual value from the lookup table
 * created in the constructor in energy_idx and dsigmal_max_val.
 */
double Interaction::KleinNishina::dsigma_max(double energy_mev)
{
    size_t idx = upper_bound(energy_idx.begin(), energy_idx.end(), energy_mev) -
                 energy_idx.begin();

    if (idx == 0) {
        return(dsigma_max_val.front());
    } else if (idx == energy_idx.size()) {
        return(dsigma_max_val.back());
    }
    double delta = energy_idx[idx] - energy_idx[idx - 1];
    double alpha = (energy_mev - energy_idx[idx - 1]) / delta;
    if (alpha > 1.0) {
        alpha = 1.0;
    }
    double dsigma_max = ((1.0 - alpha) * dsigma_max_val[idx - 1] +
                         alpha * dsigma_max_val[idx]);
    return(dsigma_max);
}


/*!
 * Calculate the pdf over it's max for use in an accept/reject monte carlo.  If
 * this is greater than or equal to a random value [0,1] then the angle theta
 * should be accepted.
 */
double Interaction::KleinNishina::dsigma_over_max(double theta,
                                                  double energy_mev)
{
    return(dsigma(theta, energy_mev) / dsigma_max(energy_mev));
}

/*!
 * For a particular energy, sweep theta for dsigma from 0 to pi in 100 steps to
 * determine the max.  100 steps is adequate for less than 0.5% error.
 */
double Interaction::KleinNishina::find_max(double energy_mev)
{
    // dsigma value is always positive, zero is safe.
    double max_val = 0;
    for (double theta = 0; theta <= M_PI; theta += (M_PI/ 100)) {
        max_val = std::max(max_val, dsigma(theta, energy_mev));
    }
    return(max_val);
}


Interaction::KleinNishina Interaction::klein_nishina;

const double Interaction::speed_of_light_cmpers = 29979245800.0;
const double Interaction::inverse_speed_of_light = (1.0 /
                                                    speed_of_light_cmpers);

double Interaction::RandomExponentialDistance(double mu) {
    double r = Random::Uniform();
    if (r > 0.0) {
        return(- log(r) / mu);
    } else {
        return(DBL_MAX);
    }
}

/*!
 * Takes an energy (MeV) and uses that to calculate if there was an interaction
 * or not by calling RandomExponentialDistance.  dist is modified to be the
 * random distance if an interaction occurs.
 */
Interaction::INTER_TYPE Interaction::InteractionType(
        Photon & photon,
        double & dist,
        const GammaStats & mat_gamma_prop,
        double & deposit)
{
    if (!mat_gamma_prop.enable_interactions) {
        // move photon to interaction point, or exit point of material
        photon.pos += (dist * photon.dir.Normalize());
        photon.time += (dist * inverse_speed_of_light);
        return(NO_INTERACTION);
    }

    double pe, compton, rayleigh;
    mat_gamma_prop.GetInteractionProbs(photon.energy, pe, compton, rayleigh);

    // TODO: add back in rayleigh scattering once the distribution is fixed
    // double rand_dist = RandomExponentialDistance(pe + compton + rayleigh);
    double rand_dist = RandomExponentialDistance(pe + compton);
    if (dist > rand_dist) {
        dist = rand_dist;
    }

    // move photon to interaction point, or exit point of material
    photon.pos += (dist * photon.dir.Normalize());
    photon.time += (dist * inverse_speed_of_light);
    if (dist < rand_dist || !mat_gamma_prop.enable_interactions) {
        return(NO_INTERACTION);
    }


    // TODO: add back in rayleigh scattering once the distribution is fixed
    // double rand = (pe + compton + rayleigh) * Random::Uniform();
    double rand = (pe + compton) * Random::Uniform();
    if (rand <= pe) {
        if (XrayEscape(photon, mat_gamma_prop, deposit)) {
            // TODO: Get x-ray escape physics working again
            return XRAY_ESCAPE;
        } else {
            return PHOTOELECTRIC;
        }
    } else if (rand <= (pe + compton)) {
        // perform compton kinematics
        ComptonScatter(photon, deposit);
        return COMPTON;
    } else {
        // perform rayleigh kinematics
        RayleighScatter(photon);
        return RAYLEIGH;
    }
}

void Interaction::KleinNishinaAngle(double energy, double & theta,
                                      double & phi)
{
    /* Generate scattering angles - phi and theta */
    // Theta is the compton angle
    do {
        theta = M_PI * Random::Uniform();
    } while (klein_nishina.dsigma_over_max(theta, energy) < Random::Uniform());

    // phi is symmetric around a circle of 360 degrees
    phi = 2 * M_PI * Random::Uniform();
}

double Interaction::KleinNishinaEnergy(double energy, double theta)
{
    return(energy / (1.0 + (energy / ENERGY_511) * (1. - cos(theta))));
}

void Interaction::ComptonScatter(Photon &p, double & deposit)
{
    double theta, phi;
    KleinNishinaAngle(p.energy, theta, phi);
    // After collision the photon loses some energy to the electron
    deposit = p.energy;
    p.energy = KleinNishinaEnergy(p.energy, theta);
    deposit -= p.energy;

    // Create rotation axis this is perpendicular to Y axis
    // to generate the scattering angle theta
    RotationMapR3 rotation;
    VectorR3 rot_axis = p.dir;
    VectorR3 UnitY;
    UnitY.SetUnitY();
    rot_axis *= UnitY;
    rot_axis.Normalize();

    // save direction for phi rotatation
    VectorR3 comp_dir = p.dir;

    // rotate incline
    rotation.Set(rot_axis, theta);
    rotation.Transform(&comp_dir);

    // rotate theta using original direction as axis
    p.dir.Normalize();
    rotation.Set(p.dir,phi);
    rotation.Transform(&comp_dir);

    // next direction is from compton scattering angle
    p.dir = comp_dir;

    // If the photon scatters on a non-detector, it is a scatter, checked
    // inside SetScatter
    p.SetScatterCompton();
}

/*!
 * The pdf of rayleigh scattering as a function of theta, normalized to a max
 * of one so that it can be used with an accept/reject method.
 */
double Interaction::RayleighProbability(double theta) {
    double cs = cos(theta);
    return((1.0 - cs * cs) / 2.0);
}

void Interaction::RayleighScatter(Photon &p)
{
    // FIXME: This implements Thompson scattering, not Rayleigh scattering
    double theta = M_PI * Random::Uniform();
    while (RayleighProbability(theta) < Random::Uniform()) {
        theta = M_PI * Random::Uniform();
    }

    // phi is symmetric around a circle of 360 degrees
    double phi = 2 * M_PI * Random::Uniform();

    // Create rotation axis this is perpendicular to Y axis
    // to generate the scattering angle theta
    RotationMapR3 rotation;
    VectorR3 rot_axis = p.dir;
    VectorR3 UnitY;
    UnitY.SetUnitY();
    rot_axis *= UnitY;
    rot_axis.Normalize();

    // save direction for phi rotatation
    VectorR3 comp_dir = p.dir;

    // rotate incline
    rotation.Set(rot_axis, theta);
    rotation.Transform(&comp_dir);

    // rotate theta using original direction as axis
    p.dir.Normalize();
    rotation.Set(p.dir,phi);
    rotation.Transform(&comp_dir);

    // next direction is from scattering angle
    p.dir = comp_dir;

    // If the photon scatters on a non-detector, it is a scatter, checked
    // inside SetScatter
    p.SetScatterRayleigh();
}

bool Interaction::XrayEscape(Photon &p, const GammaStats & mat_gamma_prop,
                             double & deposit)
{
    const std::vector<double> & emit_e = mat_gamma_prop.GetXrayEmissionEnergies();
    const std::vector<double> & prob_e = mat_gamma_prop.GetXrayEmissionCumProb();
    double rand = Random::Uniform();
    rand *= mat_gamma_prop.GetXrayBindEnergyScale(p.energy);
    size_t idx = lower_bound(prob_e.begin(), prob_e.end(), rand) - prob_e.begin();
    double xray_energy = emit_e[idx];
    if (xray_energy == 0) {
        return(false);
    } else {
        return(false);
        deposit = p.energy - xray_energy;
        p.energy = xray_energy;
        Random::UniformSphere(p.dir);
        p.SetXrayFlouresence();
        return(true);
    }
}
