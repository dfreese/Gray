#include <Output/Output.h>
#include <Physics/Interaction.h>

using namespace std;

bool Output::log_data = false;
bool Output::log_positron = false;
bool Output::log_all = false;
bool Output::binary_output = false;
Output::BinaryOutputFormat Output::binary_format = Output::FULL_OUTPUT;

Output::Output(const std::string & logfile)
{
    if (logfile != "") {
        SetLogfile(logfile);
    }
    counter_nuclear_decay = 0;
    counter_photoelectric = 0;
    counter_compton = 0;
    counter_error = 0;
}

Output::~Output()
{
    if (log_data) {
        log_file.close();
    }
    if (counter_nuclear_decay || counter_photoelectric ||
        counter_compton || counter_error || counter_rayleigh)
    {
        cout << "Nuclear Decays: " << counter_nuclear_decay << endl;
        cout << "Photoelectrics: " << counter_photoelectric << endl;
        cout << "Comptons      : " << counter_compton << endl;
        cout << "Rayleighs     : " << counter_rayleigh << endl;
        cout << "Errors        : " << counter_error << endl;
    }
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


void Output::SetBinaryFormat(BinaryOutputFormat format) {
    binary_format = format;
}

void Output::LogInteraction(const Interaction & interact) {
    bool log_event = ((log_data & interact.sensitive_mat) | log_all |
            (log_positron & (interact.type == Interaction::NUCLEAR_DECAY))) &
            (interact.type != Interaction::NO_INTERACTION);
    if (!log_event) {
        return;
    }
    switch (interact.type) {
        case Interaction::PHOTOELECTRIC:
            counter_photoelectric++;
            break;
        case Interaction::COMPTON:
            counter_compton++;
            break;
        case Interaction::RAYLEIGH:
            counter_rayleigh++;
            break;
        case Interaction::NUCLEAR_DECAY:
            counter_nuclear_decay++;
            break;
        default:
            break;
    }
    if (binary_output) {
        if (binary_format == FULL_OUTPUT) {
            GrayBinaryStandard b;
            b.i = interact.id;
            b.time = interact.time;
            b.energy = interact.energy;
            b.x = (float) interact.pos.x;
            b.y = (float) interact.pos.y;
            b.z = (float) interact.pos.z;
            b.det_id = interact.det_id;
            b.log = MakeLogWord(interact.type, interact.color,
                                interact.scatter, interact.mat_id,
                                interact.src_id);
            log_file.write(reinterpret_cast<char*>(&b), sizeof(b));
        } else if (binary_format == NO_POS) {
            GrayBinaryNoPosition b;
            b.i = interact.id;
            b.time = interact.time;
            b.energy = interact.energy;
            b.det_id = interact.det_id;
            b.log = MakeLogWord(interact.type, interact.color,
                                interact.scatter, interact.mat_id,
                                interact.src_id);
            log_file.write(reinterpret_cast<char*>(&b), sizeof(b));
        }
    } else {
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
        if (interact.scatter) {
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
        log_data = true;
        return true;
    }
}

void Output::SetLogAll(bool val)
{
    log_all = val;
}

void Output::SetLogPositron(bool val)
{
    log_positron = val;
}

void Output::SetBinary(bool val)
{
    binary_output = val;
}

bool Output::GetLogPositron() const {
    return(log_positron);
}
