#ifndef CSE167_PHOTON_H
#define CSE167_PHOTON_H

#include <VrMath/LinearR3.h>

class Photon
{
public:
    enum Color {
        P_BLUE = 0,
        P_RED = 1,
        P_YELLOW = 2
    };

    Photon();
    Photon(const VectorR3 & pos, const VectorR3 & dir, double energy,
           double time, int id, Color color, int src_id);

    void SetScatterCompton();
    void SetScatterRayleigh();
    void SetXrayFlouresence();
    const VectorR3 & GetPos() const {
        return (pos);
    }
    void AddPos(const VectorR3 & rhs) {
        pos += rhs;
    }
    const VectorR3 & GetDir() const {
        return (dir);
    }
    void SetDir(const VectorR3 & dir) {
        this->dir = dir;
    }
    double GetEnergy() const {
        return (energy);
    }
    void SetEnergy(double energy) {
        this->energy = energy;
    }
    double GetTime() const {
        return (time);
    }
    void AddTime(double rhs) {
        time += rhs;
    }
    int GetId() const {
        return (id);
    }
    Color GetColor() const {
        return (color);
    }
    int GetDetId() const {
        return (det_id);
    }
    int GetScatterComptonPhantom() const {
        return (scatter_compton_phantom);
    }
    int GetScatterComptonDetector() const {
        return (scatter_compton_detector);
    }
    int GetScatterRayleighPhantom() const {
        return (scatter_rayleigh_phantom);
    }
    int GetScatterRayleighDetector() const {
        return (scatter_rayleigh_detector);
    }
    int GetXrayFlouresence() const {
        return (xray_flouresence);
    }
    void SetDetId(int det_id) {
        this->det_id = det_id;
    }
    int GetSrc() const {
        return src_id;
    }
private:
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
