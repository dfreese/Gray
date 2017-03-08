#ifndef DETECTOR_H
#define DETECTOR_H
#include <VrMath/LinearR3.h>
#include <VrMath/LinearR4.h>

class Photon;

class Detector
{
public:
    Detector();
    Detector(unsigned id, const VectorR3 & p, const VectorR3 &s,
             const RigidMapR3 & m);
    Detector(unsigned id, const VectorR3 & p, const VectorR3 &s,
             const RigidMapR3 & m, const unsigned x, const unsigned y,
             const unsigned z, const unsigned bl);
    void Init(unsigned id, const VectorR3 & p, const VectorR3 &s,
              const RigidMapR3 & m, const unsigned x, const unsigned y,
              const unsigned z, const unsigned bl);
    unsigned detector_id;
    VectorR3 size;
    VectorR3 pos;
    RigidMapR3 map;
    friend ostream& operator<< ( ostream& os, const Detector& d );
    void SetDecayTime(double t);
    void IncrementHit();
    void SetBlock(unsigned int bl);
    unsigned long GetHitCount() const;
    double decay_time;
    unsigned idx[3];
    unsigned block;

private:
    unsigned long count;
};

#endif /*DETECTOR_*/
