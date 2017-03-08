#include <Output/Output.h>
#include <Physics/GammaStats.h>

Output::Output()
{
    log_data = false;
    log_positron = false;
    log_all = false;
    binary_output = false;
    binary_format = FULL_OUTPUT;
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
        counter_compton || counter_error)
    {
        cout << "Nuclear Decays: " << counter_nuclear_decay << endl;
        cout << "Photoelectrics: " << counter_photoelectric << endl;
        cout << "Comptons      : " << counter_compton << endl;
        cout << "Errors        : " << counter_error << endl;
    }
}

int Output::MakeLogWord(int interaction, int color, bool scatter, int det_mat, int src_id)
{
    int errorbit=0;
    if (interaction < 0) {
        errorbit = 0x80000000 ;
        interaction *= -1;
    }
    return (errorbit |
            ((interaction << 28) & (0x70000000)) |
            ((color << 26)       & (0x0C000000)) |
            ((scatter << 24)     & (0x01000000)) |
            ((det_mat << 12)     & (0x00FFF000)) |
            (src_id              & (0x00000FFF)));
}

void Output::SetBinaryFormat(BinaryOutputFormat format) {
    binary_format = format;
}

void Output::LogNuclearDecay(NuclearDecay *p )
{
    if (log_positron) {
        counter_nuclear_decay++;
        if (binary_output) {
            LogNuclearDecayBinary(p);
        } else {
            LogNuclearDecayASCII(p);
        }
    }
}

void Output::LogNuclearDecayASCII(NuclearDecay *p)
{
    log_file << " 0 ";
    log_file << *p;
    char str[256];
    if (log_all) {
        sprintf(str,"%2d %2d %2d ", p->GetSourceNum(),-2,-2);
        log_file << str;
    }
    log_file << " -1";
    log_file << "\n";
}

void Output::LogNuclearDecayBinary(NuclearDecay *p)
{
    if (binary_format == FULL_OUTPUT) {
        GRAY_BINARY b;
        b.log = MakeLogWord( 0, 1, 0, 0, p->source_num);
        b.i = p->decay_number;
        b.time = p->time;
        b.energy = p->energy;
        b.x = (float) p->pos.x;
        b.y = (float) p->pos.y;
        b.z = (float) p->pos.z;
        b.det_id = -1;
        write(b);
    } else if (binary_format == NO_POS) {
        BinaryDetectorOutput b;
        b.log = MakeLogWord( 0, 1, 0, 0, p->source_num);
        b.i = p->decay_number;
        b.time = p->time;
        b.energy = p->energy;
        b.det_id = -1;
        write(b);
    }
}

void Output::LogCompton(const Photon &p, double deposit, const GammaStats & mat_gamma_prop)
{
    bool log_event = (log_data & mat_gamma_prop.log_material) | log_all;
    if (log_event) {
        if (binary_output) {
            LogBinary(p, COMPTON, deposit, mat_gamma_prop);
        } else {
            LogASCII(p, COMPTON, deposit,mat_gamma_prop);
        }
        counter_compton++;
    }
}


void Output::LogPhotoElectric(const Photon &p, const GammaStats & mat_gamma_prop)
{
    bool log_event = (log_data & mat_gamma_prop.log_material) | log_all;
    if (log_event) {
        if (binary_output) {
            LogBinary(p,PHOTOELECTRIC, p.energy, mat_gamma_prop);
        } else {
            LogASCII(p,PHOTOELECTRIC, p.energy, mat_gamma_prop);
        }
        counter_photoelectric++;
    }
}


void Output::LogASCII(const Photon &p, INTER_TYPE type, double deposit, const GammaStats & mat_gamma_prop)
{
    Photon ptmp;
    ptmp = p;
    ptmp.energy = deposit;
    if (type == COMPTON ) {
        log_file << " " << 1 << " ";
    } else {
        if (type == PHOTOELECTRIC ) {
            log_file << " " << 3 << " ";
        } else {
            log_file << " " << 5 << " ";
        }
    }
    log_file << ptmp;
    char str[256];
    sprintf(str,"%2d ",mat_gamma_prop.GetMaterial());
    log_file << str;
    sprintf(str,"%3d ",p.det_id );
    log_file << str;
    log_file << "\n";

}

void Output::LogBinary(const Photon &p, INTER_TYPE type, double deposit, const GammaStats & mat_gamma_prop)
{
    if (binary_format == FULL_OUTPUT) {
        GRAY_BINARY b;
        b.log = MakeLogWord( (int) type, p.color, p.phantom_scatter ,  mat_gamma_prop.GetMaterial() ,p.src_id);
        b.i = p.id;
        b.time = p.time;
        b.energy = deposit;
        b.x = (float) p.pos.x;
        b.y = (float) p.pos.y;
        b.z = (float) p.pos.z;
        b.det_id = p.det_id;
        write(b);
    } else if (binary_format == NO_POS) {
        BinaryDetectorOutput b;
        b.log = MakeLogWord( (int) type, p.color, p.phantom_scatter ,  mat_gamma_prop.GetMaterial() ,p.src_id);
        b.i = p.id;
        b.time = p.time;
        b.energy = deposit;
        b.det_id = p.det_id;
        write(b);
    }
}


void Output::LogError(const Photon &p, ErrorType t, int det_mat)
{
    counter_error++;
    if (binary_output) {
        LogErrorBinary(p, t, det_mat);
    } else {
        LogErrorASCII(p, t, det_mat);
    }
}

void Output::LogErrorASCII(const Photon &p, ErrorType t, int detmaterial)
{
    log_file <<   t;
    log_file << " ";
    log_file << p;
    char str[256];
    sprintf(str,"%2d ", detmaterial);
    log_file << str;
    sprintf(str,"%3d ",p.det_id );
    log_file << str;
    log_file << "\n";
}


/// ADD  mat to function call
void Output::LogErrorBinary(const Photon &p, ErrorType t, int detmaterial )
{
    if (binary_format == FULL_OUTPUT) {
        GRAY_BINARY b;
        b.log = MakeLogWord( t, p.color, p.phantom_scatter , detmaterial, p.src_id);
        b.i = p.id;
        b.time = p.time;
        b.energy = p.energy;
        b.x = p.pos.x;
        b.y = p.pos.y;
        b.z = p.pos.z;
        write(b);
    } else if (binary_format == NO_POS) {
        BinaryDetectorOutput b;
        b.log = MakeLogWord(t, p.color, p.phantom_scatter , detmaterial, p.src_id);
        b.i = p.id;
        b.time = p.time;
        b.energy = p.energy;
        write(b);
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
        //eb.SetSingles(name);
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

void Output::write(GRAY_BINARY& data)
{
    log_file.write(reinterpret_cast<char*>(&data), sizeof(GRAY_BINARY));
}

void Output::write(BinaryDetectorOutput & data)
{
    log_file.write(reinterpret_cast<char*>(&data), sizeof(BinaryDetectorOutput));
}
