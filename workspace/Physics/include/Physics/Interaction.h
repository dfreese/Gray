#ifndef CSE167_INTERACTION_H
#define CSE167_INTERACTION_H

#include <iostream>
#include <fstream>
#include <Physics/Photon.h>

class GammaStats;

using namespace std;

//#define XRAY_ESCAPE_ENERGY_LSO 63.314
//#define XRAY_ESCAPE_ENERGY_CD 26.711
//#define XRAY_ESCALE_ENERGY_TE 31.814
// TODO DO CZT XRAY_ESCAPE PHYSICS

class Interaction {
public:
    static INTER_TYPE GammaInteraction(Photon &p, double dist, const GammaStats & mat_gamma_prop);
    static bool GammaAttenuation(double &dist, double u_attentuation);
    static INTER_TYPE PE(double sigma, double mu, Photon &p, const GammaStats & mat_gamma_prop);
    static void Klein_Nishina(double dsdom, Photon &p, const GammaStats & mat_gamma_prop);
    static double dsigma(double phi, double alpha);
    static bool XrayEscape(Photon &p, const GammaStats & mat_gamma_prop);
    static const double si1_SOL;
};

#endif
