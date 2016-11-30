#include "VoxelSource.h"

VoxelSource::~VoxelSource() {
	for (int i; i < index.size(); i++) {
		delete index[i];
	}
}

VoxelSource::VoxelSource() {
	position.SetZero();
	scale.SetZero();
	dimension[0] = 0; dimension[1] = 0; dimension[2] = 0;
}

VoxelSource::VoxelSource(const VectorR3 &p, int dims[3], const VectorR3 & vox_scale, double act) {
	SetActivity(act);
	dimension[0] = dims[0]; dimension[1] = dims[1]; dimension[2] = dims[2];
	scale = vox_scale;
	position = p;

	// Divide the size of the voxel_array in cm by dimensions to allow for correct positioning
	scale.x = scale.x / (double)dims[0];
	scale.y = scale.y / (double)dims[1];
	scale.z = scale.z / (double)dims[2];
       


	// Center array
	position.x -= scale.x * ((double)dims[0])/2.0;
	position.y -= scale.y * ((double)dims[1])/2.0;
	position.z -= scale.z * ((double)dims[2])/2.0;
}


int VoxelSource::search(double e, int b_idx, int s_idx) {

	if (b_idx == s_idx) {
		return b_idx;
	}
	int idx = (int)(((b_idx) + (s_idx))/2);
	//	cout << " e = " << e << " b_idx = " << b_idx<< " s_idx = " << s_idx << " prob[" << idx << "] = " << prob[idx] << endl;
	if (prob[idx] < e) return search(e,idx+1,s_idx);
	else return search(e,b_idx,idx);
}

void VoxelSource::Decay(unsigned int photon_number) {
	if (isotope == NULL) return;

	VectorR3 pos;
	// Random is uniformly distributed between 0 and 1
        // prob.size is number of voxels. 
	int idx = search(Random(),0,prob.size()-1);
	pos.x = scale.x * (index[idx][0]+Random());
	pos.y = scale.y * (index[idx][1]+Random());
	pos.z = scale.z * (index[idx][2]+Random());

	pos += position;

	/* Debugging code 
		cout << "DONE ::  pos = " <<pos ;
                cout << " position = " << position;
          cout << " idx = " << idx ;
          cout << " index[" << idx << "][0] = " << index[idx][0] ;
          cout << " index[" << idx << "][1] = " << index[idx][1] ;
	  cout << " index[" << idx << "][2] = " << index[idx][2] << endl;
*/

	isotope->SetMaterial(GetMaterial());
	isotope->SetPosition(pos);
	isotope->Decay(photon_number);
}

bool VoxelSource::Load(const char * filename) {
	ifstream file;	
	file.open(filename);

	if (!file) {
		cout << "Error loading file: " << filename << endl;
		return false;
	}

	int i,j,k;
	double val;

	int * idx;
	double total=0;
	cout << "Opening voxel array:" << filename << endl;
	cout << " voxel size: " << scale.x << " x " << scale.y << " x ";
        cout << scale.z << endl;

	for (k = 0; k < dimension[2]; k++) {
		for (j = 0; j < dimension[1]; j++) {
			for (i = 0; i < dimension[0]; i++) {
				file >> val;
				if (file.fail()) {
					cout << "Error reading file at ["<< i << "][" << j << "][" << k << "]\n";
					return false;
				}
				
				//	cout << "Reading["<< i << "][" << j << "][" << k << "]= " << val << endl;
				if (val > 0) {
				total += val;
				prob.push_back(total);
				idx = new int[3];
				idx[0] = i; idx[1] = j; idx[2] = k;
				index.push_back(idx);
				}
			}
		}
	}
	cout << "done.\n";

	for (i = 0; i < prob.size(); i++) {
		prob[i] /= total;
	}

	file.close();

	return true;
}

bool VoxelSource::Inside(const VectorR3 & pos) const {
	if (isotope == NULL) return false;
	return false;
}
