#include <Physics/Photon.h>
#include <stdio.h>

using namespace std;

Photon::Photon()
{
    Reset();
}

void Photon::Reset()
{
    energy = -1.0;
    time = 0.0;
    dir.SetZero();
    pos.SetZero();
    id = 0;
    src_id = -1;
    det_id = -1;
    color = P_BLUE;
    scatter_compton_phantom = 0;
    scatter_compton_detector = 0;
    scatter_rayleigh_phantom = 0;
    scatter_rayleigh_detector = 0;
    xray_flouresence = 0;
}

Photon::Photon(int set_id, double e, const VectorR3 & p, const VectorR3 & d) :
    pos(p),
    dir(d),
    energy(e),
    time(0.0),
    id(set_id),
    color(P_BLUE),
    det_id(-1),
    scatter_compton_phantom(0),
    scatter_compton_detector(0),
    scatter_rayleigh_phantom(0),
    scatter_rayleigh_detector(0),
    xray_flouresence(0),
    src_id(-1)
{
}

void Photon::SetScatterCompton()
{
    if (det_id == -1) {
        scatter_compton_phantom++;
    } else {
        scatter_compton_detector++;
    }
}

void Photon::SetScatterRayleigh()
{
    if (det_id == -1) {
        scatter_compton_phantom++;
    } else {
        scatter_compton_detector++;
    }
}

void Photon::SetXrayFlouresence() {
    xray_flouresence++;
}
