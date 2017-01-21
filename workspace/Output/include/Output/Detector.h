#ifndef DETECTOR_H
#define DETECTOR_H
#include <VrMath/LinearR3.h>
#include <VrMath/LinearR4.h>

class Photon;

class Detector
{
public:
    Detector();
    Detector(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres);
    Detector(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres, const unsigned x, const unsigned y, const unsigned z, const unsigned bl);
    void Init(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres, const unsigned x, const unsigned y, const unsigned z,const unsigned bl);
    unsigned detector_id;
    VectorR3 size;
    VectorR3 pos;
    RigidMapR3 map;
    friend ostream& operator<< ( ostream& os, const Detector& d );
    void SetTimeResolution(double r);
    void SetEnergyResolution(double r);
    void SetDecayTime(double t);
    double GetTimeResolution() const;
    double GetEnergyResolution() const;
    double BlurTime(const Photon &p );
    double BlurEnergy(const Photon &p );
    void IncrementHit();
    void SetBlock(unsigned int bl);
    unsigned long GetHitCount() const;
    double decay_time;
    unsigned idx[3];
    unsigned block;

private:
    double time_resolution;	/* specified as ns FWHM */
    double energy_resolution; /* specified as % FWHM */
    unsigned long count;
};

#endif /*DETECTOR_*/
