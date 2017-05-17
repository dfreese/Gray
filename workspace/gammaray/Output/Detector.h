#ifndef DETECTOR_H
#define DETECTOR_H
#include <VrMath/LinearR3.h>
#include <VrMath/LinearR4.h>

class Detector
{
public:
    Detector();
    Detector(int id, const VectorR3 & p, const VectorR3 &s,
             const RigidMapR3 & m);
    Detector(int id, const VectorR3 & p, const VectorR3 &s,
             const RigidMapR3 & m, int x, int y, int z, int bl);
    void Init(int id, const VectorR3 & p, const VectorR3 &s,
              const RigidMapR3 & m, int x, int y, int z, int bl);
    int detector_id;
    VectorR3 size;
    VectorR3 pos;
    RigidMapR3 map;
    friend ostream& operator<< ( ostream& os, const Detector& d );
    void SetBlock(int bl);
    int idx[3];
    int block;
};

#endif /*DETECTOR_*/