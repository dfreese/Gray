#ifndef CSE167_PHOTON_H
#define CSE167_PHOTON_H

#include <VrMath/MathMisc.h>
#include <VrMath/LinearR3.h>

#define ENERGY_511 0.510998903

class Photon
{
public:
    Photon();
    Photon(int set_id, double e, const VectorR3 & p, const VectorR3 & d);
    enum Color {
        P_BLUE = 0,
        P_RED = 1,
        P_YELLOW = 2
    };
    void SetRed()
    {
        color = P_RED;
    }
    void SetBlue()
    {
        color = P_BLUE;
    }
    void SetSrc(int val)
    {
        src_id=val;
    }
    int GetSrc(void) const
    {
        return src_id;
    }
    void Reset();
    void SetScatterCompton();
    void SetScatterRayleigh();
    void SetXrayFlouresence();

public:
    VectorR3 pos;
    VectorR3 dir;
    double energy;
    double time;
    int id;
    Color color;
    int det_id;
    int scatter_compton_phantom;
    int scatter_compton_detector;
    int scatter_rayleigh_phantom;
    int scatter_rayleigh_detector;
    int xray_flouresence;
    int src_id;
};

#endif
