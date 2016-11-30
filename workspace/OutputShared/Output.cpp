#include "Output.h"

Output::Output() {
	log_data = false;
	log_positron = true;
	log_all = false;
}

Output::~Output() {
	if (log_data)
		log_file.close();
}

void Output::LogPositron(PositronDecay &p) {
	if (log_data&log_positron) {
		log_file << " 0 ";
		log_file << p;
		if (log_all) {
			log_file << "0";
		}
		log_file << "\n";
	}
}

void Output::LogCompton(Photon &p, double deposit, const MaterialBase & mat) {
	bool log_event = (log_data&mat.GammaProp->log_material)|log_all;
	if (log_event) {
		Photon ptmp = p;
		ptmp.energy = deposit;
		log_file << " 1 ";
		log_file << ptmp;
		if (log_all) {
			log_file << " ";
			log_file << mat.GammaProp->log_material;
		}
		log_file << "\n";
	}
}

void Output::LogPhotoElectric(Photon &p, double deposit, const MaterialBase & mat) {
		bool log_event = (log_data&mat.GammaProp->log_material)|log_all;
		if (log_event) {
			log_file << " 3 ";
			log_file << p;
			if (log_all) {
				log_file << " ";
				log_file << mat.GammaProp->log_material;
			}
			log_file << "\n";
		}
}

bool Output::SetLogfile(char * name) {
	log_file.open(name, ios::out);
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

void Output::SetLogAll(bool val) {
	log_all = val;
}

