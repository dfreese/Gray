#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <Physics/Photon.h>
#include <Physics/Interaction.h>

class GammaStats;
class NuclearDecay;

class Physics {
public:
    // FIXME: interaction type is used in the binary output, but only given 3
    // bits, so anything more than 7 will cause an error.
    enum INTER_TYPE {
        COMPTON = 0,
        PHOTOELECTRIC = 1,
        XRAY_ESCAPE = 2,
        RAYLEIGH = 3,
        NO_INTERACTION = 4,
        NUCLEAR_DECAY = 5,
        ERROR_EMPTY = 6,
        ERROR_TRACE_DEPTH = 7,
    };

    Physics() {};

    static Interaction NoInteraction();
    static Interaction NoInteraction(const Photon & p,
                                     const GammaStats & mat_gamma_prop);
    static Interaction Photoelectric(const Photon & p,
                                     const GammaStats & mat_gamma_prop);
    static Interaction XrayEscape(const Photon & p, double deposit,
                                  const GammaStats & mat_gamma_prop);
    static Interaction Compton(const Photon & p, double deposit,
                               const GammaStats & mat_gamma_prop);
    static Interaction Rayleigh(const Photon & p,
                                const GammaStats & mat_gamma_prop);
    static Interaction NuclearDecay(const NuclearDecay & p,
                                    const GammaStats & mat_gamma_prop);
    static Interaction ErrorTraceDepth(const Photon & p,
                                       const GammaStats & mat_gamma_prop);
    static Interaction ErrorEmtpy(const Photon & p);

    static INTER_TYPE InteractionType(Photon &p,
                                      double & dist,
                                      const GammaStats & mat_gamma_prop,
                                      double & deposit);
    static void ComptonScatter(Photon &p, double & deposit);
    static void KleinNishinaAngle(double energy, double & theta,
                                  double & phi, double & prob_e_theta);
    static double KleinNishinaEnergy(double energy, double theta);
    static bool XrayEscape(Photon &p, const GammaStats & mat_gamma_prop,
                           double & deposit);
    static void RayleighScatter(Photon &p);
    static const double speed_of_light_cmpers;
    static const double inverse_speed_of_light;

private:
    // A class for static initialization of the dsigma_max values as a function
    // of energy
    class KleinNishina {
    public:
        KleinNishina();
        double dsigma_max(double energy_mev);
        double dsigma_over_max(double theta, double energy_mev,
                               double & prob_e_theta);
        static double dsigma(double theta, double energy_mev,
                             double & prob_e_theta);
    private:
        static double find_max(double energy_mev);
        std::vector<double> energy_idx;
        std::vector<double> dsigma_max_val;
    };
    static KleinNishina klein_nishina;
    static double RayleighProbability(double theta);
};

#endif // PHYSICS_H
