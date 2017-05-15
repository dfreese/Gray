#ifndef CSE167_VOXELSOURCE_H
#define CSE167_VOXELSOURCE_H

#include <Sources/Source.h>
#include <vector>

class VoxelSource : public Source
{
public:
    ~VoxelSource();
    VoxelSource();
    VoxelSource(const VectorR3 &p, int dims[3], const VectorR3 & vox_scale, double act);
    bool Load(const char * filename);

    virtual VectorR3 Decay(int photon_number, double time);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    int dimension[3];
    VectorR3 scale;
    size_t search(double e, size_t b_idx, size_t s_idx);
    int AddSource(double val);

    std::vector <double> prob;
    std::vector <int*> index;
};

#endif
