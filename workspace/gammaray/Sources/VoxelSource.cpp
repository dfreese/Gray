#include <Sources/VoxelSource.h>
#include <iostream>
#include <fstream>
#include <Random/Random.h>

using namespace std;

VoxelSource::~VoxelSource()
{
    for (int i = 0; i < index.size(); i++) {
        delete index[i];
    }
}

VoxelSource::VoxelSource() :
    Source(),
    dimension{0, 0, 0},
    scale(0, 0, 0)
{
}

VoxelSource::VoxelSource(const VectorR3 &p, int dims[3], const VectorR3 & vox_scale, double act) :
    Source(p, act),
    dimension{dims[0], dims[1], dims[2]},
    scale(vox_scale)
{
    // Divide the size of the voxel_array in cm by dimensions to allow for correct positioning
    scale.x = scale.x / (double)dims[0];
    scale.y = scale.y / (double)dims[1];
    scale.z = scale.z / (double)dims[2];

    // Center array
    position.x -= scale.x * ((double)dims[0])/2.0;
    position.y -= scale.y * ((double)dims[1])/2.0;
    position.z -= scale.z * ((double)dims[2])/2.0;
}


size_t VoxelSource::search(double e, size_t b_idx, size_t s_idx)
{

    if (b_idx == s_idx) {
        return b_idx;
    }
    size_t idx = ((b_idx) + (s_idx)) / 2;
    if (prob[idx] < e) {
        return search(e,idx+1,s_idx);
    } else {
        return search(e,b_idx,idx);
    }
}

VectorR3 VoxelSource::Decay(int photon_number, double time)
{
    if (isotope == NULL) {
        return(VectorR3(0, 0, 0));
    }

    VectorR3 pos;
    // Random is uniformly distributed between 0 and 1
    // prob.size is number of voxels.
    size_t idx = search(Random::Uniform(), 0, prob.size() - 1);
    pos.x = scale.x * (index[idx][0] + Random::Uniform());
    pos.y = scale.y * (index[idx][1] + Random::Uniform());
    pos.z = scale.z * (index[idx][2] + Random::Uniform());

    pos += position;

    isotope->Decay(photon_number, time, source_num, pos);
    return(pos);
}

bool VoxelSource::Load(const char * filename)
{
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

                if (val > 0) {
                    total += val;
                    prob.push_back(total);
                    idx = new int[3];
                    idx[0] = i;
                    idx[1] = j;
                    idx[2] = k;
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

bool VoxelSource::Inside(const VectorR3 & pos) const
{
    if (isotope == NULL) {
        return false;
    }
    return false;
}
