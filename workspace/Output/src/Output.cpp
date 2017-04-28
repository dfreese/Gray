#include <Output/Output.h>
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
    if (format == FULL_BINARY) {
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
    } else {
        return true;
    }
}

int Output::GetFormat(const std::string & identifier, Output::Format & fmt) {
    if (identifier == "full_ascii") {
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
