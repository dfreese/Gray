#include "Output.h"

Output::Output()
{
    log_data = false;
    log_positron = true;
    log_all = false;
    binary = false;
    binning = false;
    log_det_id = false;
    log_det_coord = false;
    cout << "Size of GRAY STRUCT: ";
    cout << sizeof(GRAY_BINARY);
    cout << "\n";
}

Output::~Output()
{
    if (log_data) {
        log_file.close();
    }
}

/*  interaction will also serve as error number */
int Output::MakeLogWord(int interaction, int color, bool scatter, int det_mat, int src_id)
{
    int errorbit=0;
    if (interaction <0 ) {
        errorbit = 0x80000000 ;
        interaction*=-1;
    }
    return (  errorbit | ( ( interaction << 28) & (0x70000000))   | ( (color << 26 ) & 0xC000000 )| ((scatter << 24) &0x1000000 )| ( ( det_mat << 12 ) & (0xFFF000)) | ( src_id & 0xFFF ) );
}

void Output::LogNuclearDecay(NuclearDecay *p )
{
    if (log_positron) {
        if (binary) {
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
    if (log_det_id) {
        log_file << " -1";
    }
    //        log_file << " " << p->GetSourceNum();
    //	sprintf(str," 0x%x", MakeLogWord( 0, 1, 0, 0, p->source_num));
    //        log_file << str ;
    log_file << "\n";
}

void Output::LogNuclearDecayBinary(NuclearDecay *p)
{
    GRAY_BINARY b;
    //	b.t = 0;
    //FIXME ____ CHECK WHAT p->decay_number really is --> OK, I think it's the event number
    b.log = MakeLogWord( 0, 1, 0, 0, p->source_num);
    b.i = p->decay_number;
//	b.rnb = (uint8_t)p.color;
//	b.rnb = 0;
    b.time = p->time;
    b.energy = p->energy;
    b.x = (float) p->pos.x;
    b.y = (float) p->pos.y;
    b.z = (float) p->pos.z;
    b.det_id = -1;
    //	b.m = 0;
    write(b);
}

void Output::LogCompton(const Photon &p, double deposit, const MaterialBase & mat)
{
    bool log_event = (log_data&mat.GammaProp->log_material)|log_all;
    if (log_event) {
        if (binary) {
            LogBinary(p, COMPTON, deposit, mat);
        } else {
            LogASCII(p, COMPTON, deposit,mat);
        }
        //	  if (binary) LogComptonBinary(p,  deposit, mat);
        //	  else LogComptonASCII(p,  deposit,mat);

    }
}


void Output::LogPhotoElectric(const Photon &p, const MaterialBase & mat)
{
    bool log_event = (log_data&mat.GammaProp->log_material)|log_all;
    //	cout << " log_data = " << log_data << " log_event = " << log_event << endl;
    if (log_event) {
        if (binary) {
            LogBinary(p,PHOTOELECTRIC,p.energy,mat);
        } else {
            LogASCII(p,PHOTOELECTRIC,p.energy,mat);
        }
    }
}


void Output::LogASCII(const Photon &p, INTER_TYPE type, double deposit, const MaterialBase & mat)
{
    Photon ptmp;
    ptmp = p;
    ptmp.energy = deposit;
    if (( type == COMPTON ) || (type == PHOTOELECTRIC)) {
        if (p.det_id != -1) {
            eb.AddEvent(ptmp, type ,mat, (d.d[p.det_id]));
            c.AddEvent(ptmp, type ,mat, (d.d[p.det_id]));
        } else {
            eb.AddEvent(ptmp, type,mat, NULL);
            c.AddEvent(ptmp, type, mat, NULL);
        }
    }
    if (!log_all) {
        return;
    }
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
    //	if (log_event) {
    //	log_file << mat.GammaProp->log_material;
    //		log_file << " ";
    sprintf(str,"%2d ",mat.GammaProp->GetMaterial());
    log_file << str;
    //	}
    if (log_det_id) {
        sprintf(str,"%3d ",p.det_id );
        log_file << str;
    }
    if(log_det_coord) {
        if (p.det_id > -1) {
            VectorR3 pos;
            pos = d.d.GetEntry(p.det_id)->pos;
            sprintf(str," %14.8e %14.8e %14.8e",pos.x, pos.y, pos.z);
        } else {
            sprintf(str," %14.8e %14.8e %14.8e",0.0f, 0.0f, 0.0f);
        }
        log_file << str;
    }
    //	sprintf(str," 0x%x", MakeLogWord(  type, p.color, p.phantom_scatter ,  mat.GammaProp->GetMaterial(),p.src_id ));
    //        log_file << str ;
    log_file << "\n";

}

void Output::LogBinary(const Photon &p, INTER_TYPE type, double deposit, const MaterialBase & mat)
{
    GRAY_BINARY b;
    b.log = MakeLogWord( (int) type, p.color, p.phantom_scatter ,  mat.GammaProp->GetMaterial() ,p.src_id);
    //	b.t = 1;
    b.i = p.id;
    //	b.rnb = (uint8_t)p.color;
    b.time = p.time;
    b.energy = deposit;
    b.x = (float) p.pos.x;
    b.y = (float) p.pos.y;
    b.z = (float) p.pos.z;
    b.det_id = p.det_id;
    // TODO: code detector ID for binary output
    //
    //	b.m = mat.GammaProp->log_material;
    write(b);
}


void Output::LogError(const Photon &p, int t, int det_mat)
{
    if (binary) {
        LogErrorBinary(p, t, det_mat);
    } else {
        LogErrorASCII(p, t, det_mat);
    }
}

void Output::LogErrorASCII(const Photon &p, int t, int detmaterial)
{
    log_file <<   t;
    log_file << " ";
    log_file << p;
    char str[256];
    //	if (log_event) {
    //	log_file << mat.GammaProp->log_material;
    //		log_file << " ";
    sprintf(str,"%2d ", detmaterial);
    log_file << str;
    //	}
    if (log_det_id) {
        sprintf(str,"%3d ",p.det_id );
        log_file << str;
    }
    if(log_det_coord) {
        if (p.det_id > -1) {
            VectorR3 pos;
            pos = d.d.GetEntry(p.det_id)->pos;
            sprintf(str," %14.8e %14.8e %14.8e",pos.x, pos.y, pos.z);
        } else {
            sprintf(str," %14.8e %14.8e %14.8e",0.0f, 0.0f, 0.0f);
        }
        log_file << str;
    }
    //	sprintf(str," 0x%x ",MakeLogWord ( t, p.color, p.phantom_scatter , detmaterial, p.src_id));
    log_file << "\n";
}


/// ADD  mat to function call
void Output::LogErrorBinary(const Photon &p, int t, int detmaterial )
{
    GRAY_BINARY b;
    b.log = MakeLogWord( t, p.color, p.phantom_scatter , detmaterial, p.src_id);
    b.i = p.id;
    b.time = p.time;
    b.energy = p.energy;
    b.x = p.pos.x;
    b.y = p.pos.y;
    b.z = p.pos.z;
    // TODO: code detector ID for binary output
    //	b.det_id = t;
    //	b.m = t;
    write(b);
}



bool Output::SetLogfile(char * name)
{
    log_file.open(name, ios::out);
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

void Output::SetBinary(bool val)
{
    binary = val;
}

void Output::SetBinning(bool val)
{
    binning = val;
}

void Output::SetLogDetId(bool val)
{
    log_det_id = val;
    eb.SetLogDetector(val);
}

void Output::SetLogDetCoord(bool val)
{
    log_det_coord = val;
}

void Output::write(GRAY_BINARY& data)
{
    log_file.write(reinterpret_cast<char*>(&data), sizeof(GRAY_BINARY));
}

/* Helper function for reading binary data
 *
 *
 */
void Output::read(const std::string& file_name, GRAY_BINARY& data)
{
    std::ifstream in(file_name.c_str());
    in.read(reinterpret_cast<char*>(&data), sizeof(GRAY_BINARY));
}
