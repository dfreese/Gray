#include <Physics/Photon.h>

Photon::Photon() :
    pos(0,0,0),
    dir(0,0,1),
    energy(-1),
    time(0.0),
    id(0),
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

Photon::Photon(const VectorR3 & pos, const VectorR3 & dir, double energy,
               double time, int id, Color color, int src_id) :
    pos(pos),
    dir(dir),
    energy(energy),
    time(time),
    id(id),
    color(color),
    det_id(-1),
    scatter_compton_phantom(0),
    scatter_compton_detector(0),
    scatter_rayleigh_phantom(0),
    scatter_rayleigh_detector(0),
    xray_flouresence(0),
    src_id(src_id)
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
        scatter_rayleigh_phantom++;
    } else {
        scatter_rayleigh_detector++;
    }
}

void Photon::SetXrayFlouresence() {
    xray_flouresence++;
}
