#ifndef CSE167_INTERACTION_H
#define CSE167_INTERACTION_H

#include <iostream>
#include <fstream>
#include <Physics/Photon.h>

class GammaMaterial;

using namespace std;

//#define XRAY_ESCAPE_ENERGY_LSO 63.314
//#define XRAY_ESCAPE_ENERGY_CD 26.711
//#define XRAY_ESCALE_ENERGY_TE 31.814
// TODO DO CZT XRAY_ESCAPE PHYSICS

const double si1_SOL = (1.0 / 29979245800.0);

class Interaction
{
public:
    Interaction();
    ~Interaction();
    INTER_TYPE GammaInteraction(Photon &p, double dist, const GammaMaterial & mat);
    bool GammaAttenuation(double &dist, double u_attentuation);
    INTER_TYPE PE(double sigma, double mu, Photon &p, const GammaMaterial & mat);
    void Klein_Nishina(double dsdom, Photon &p, const GammaMaterial & mat);
    double dsigma(double phi, double alpha);
private:
    bool XrayEscape(Photon &p, const GammaMaterial & mat);
    VectorR3 comp_pos;
};

#endif
