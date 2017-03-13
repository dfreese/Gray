#ifndef CSE167_INTERACTION_H
#define CSE167_INTERACTION_H

#include <iostream>
#include <vector>
#include <Physics/Photon.h>

class GammaStats;
class Positron;

//#define XRAY_ESCAPE_ENERGY_LSO 63.314
//#define XRAY_ESCAPE_ENERGY_CD 26.711
//#define XRAY_ESCALE_ENERGY_TE 31.814
// TODO DO CZT XRAY_ESCAPE PHYSICS

class Interaction {
public:
    enum INTER_TYPE {
        COMPTON,
        PHOTOELECTRIC,
        AUGER,
        XRAY_ESCAPE,
        RAYLEIGH,
        INTERACTION,
        NO_INTERACTION,
        ESCAPE_INTERACTION,
        ERROR,
        POSITRON
    };

    Interaction();

    INTER_TYPE type;
    int id;
    double time;
    VectorR3 pos;
    double energy;
    Photon::Color color;
    int src_id;
    int mat_id;
    int det_id;
    bool error;
    bool scatter;
    bool sensitive_mat;

    static Interaction NoInteraction();
    static Interaction Photoelectric(const Photon & p,
                                     const GammaStats & mat_gamma_prop);
    static Interaction XrayEscape(const Photon & p, double deposit,
                                  const GammaStats & mat_gamma_prop);
    static Interaction Compton(const Photon & p, double deposit,
                               const GammaStats & mat_gamma_prop);
    static Interaction Rayleigh(const Photon & p,
                                const GammaStats & mat_gamma_prop);
    static Interaction NuclearDecay(const Positron & p, double deposit,
                                    const GammaStats & mat_gamma_prop);

    static Interaction GammaInteraction(Photon &p,
                                        double dist,
                                        const GammaStats & mat_gamma_prop);
    static INTER_TYPE InteractionType(Photon &p,
                                      const GammaStats & mat_gamma_prop);
    static void Klein_Nishina(Photon &p, double & deposit);
    static void Klein_Nishina_Angle(double energy, double & theta,
                                    double & phi);
    static double Klein_Nishina_Energy(double energy, double theta);
    static bool XrayEscape(Photon &p, const GammaStats & mat_gamma_prop);
    static const double si1_SOL;
    static void Rayleigh(Photon &p);
    static bool PhotonInteracts(double energy, double & dist,
                                const GammaStats & mat_gamma_prop);
    static double RandomExponentialDistance(double mu);

private:
    // A class for static initialization of the dsigma_max values as a function
    // of energy
    class KleinNishina {
    public:
        KleinNishina();
        double dsigma_max(double energy_mev);
        double dsigma_over_max(double theta, double energy_mev);
        static double dsigma(double theta, double energy_mev);
    private:
        static double find_max(double energy_mev);
        std::vector<double> energy_idx;
        std::vector<double> dsigma_max_val;
    };
    static KleinNishina klein_nishina;
    static double RayleighProbability(double theta);
};

#endif
