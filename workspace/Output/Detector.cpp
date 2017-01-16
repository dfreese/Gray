#include <Detector.h>
#include <stdio.h>
#include <Random.h>

Detector::Detector()
{
    detector_id = 0;
    pos.Set(-1.0f, -1.0f, -1.0f);
    size.Set(-1.0f, -1.0f, -1.0f);
    time_resolution = 2.0e-9; /* default time resolution ns FWHM */
    energy_resolution = 0.1; /* default energy resolution % FWHM*/
    count = 0;
    idx[0] = 0;
    idx[1] = 0;
    idx[2] = 0;
    block = 0;
}

Detector::Detector(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres)
{
    Init(id,p,s,m,time_res,eres,0,0,0,0);
}

Detector::Detector(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres, const unsigned x, const unsigned y, const unsigned z, const unsigned bl)
{
    Init(id,p,s,m,time_res,eres,x,y,z,bl);
}

void Detector::Init(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres, const unsigned x, const unsigned y, const unsigned z, const unsigned bl)
{
    detector_id = id;
    pos = p;
    size = s;
    map = m;
    time_resolution = time_res; /* default time resolution ns FWHM */
    energy_resolution = eres; /* default energy resolution % FWHM*/
    count = 0;
    idx[0] = x;
    idx[1] = y;
    idx[2] = z;
    block = bl;
}

void Detector::SetTimeResolution(double r) {
    time_resolution = r;
}
void Detector::SetEnergyResolution(double r)
{
    energy_resolution = r;
}
void Detector::SetDecayTime(double t)
{
    decay_time = t;
}
double Detector::GetTimeResolution() const
{
    return time_resolution;
};
double Detector::GetEnergyResolution() const
{
    return energy_resolution;
};

void Detector::SetBlock(unsigned int bl)
{
    block = bl;
}
unsigned long Detector::GetHitCount() const
{
    return count;
}

double Detector::BlurTime(Photon & p)
{
    return p.time + time_resolution * FWHM_to_sigma * Random::Gaussian();
}

double Detector::BlurEnergy(Photon & p)
{
    return p.energy * (1.0 + energy_resolution * FWHM_to_sigma * Random::Gaussian());
}

void Detector::IncrementHit()
{
    count++;
}

ostream& operator<< ( ostream& os, const Detector& d )
{
    char str[256];
    os << d.detector_id;
    os << " ";
    sprintf(str,"%08e %08e %08e ",d.size.x, d.size.y, d.size.z);
    os << str;
    sprintf(str,"%08e %08e %08e ",d.pos.x, d.pos.y, d.pos.z);
    os <<  str;
    os << " ";
    os << d.idx[0];
    os << " ";
    os << d.idx[1];
    os << " ";
    os << d.idx[2];
    os << " ";
    os << d.block;
    return os;
}
