#ifndef CSE167_VOXELSOURCE_H
#define CSE167_VOXELSOURCE_H

#include <Sources/Source.h>

class VoxelSource : public Source
{
public:
    ~VoxelSource();
    VoxelSource();
    VoxelSource(const VectorR3 &p, int dims[3], const VectorR3 & vox_scale, double act);
    bool Load(const char * filename);

    void virtual Decay(unsigned int photon_number);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    int dimension[3];
    VectorR3 scale;
    int search(double e, int b_idx, int s_idx);
    int AddSource(double val);

    vector <double> prob;
    vector <int*> index;
};

#endif
