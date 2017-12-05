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
    static void ComptonScatter(Photon &p, double & deposit,
                               const GammaStats & mat_prop);
    static double KleinNishinaEnergy(const double energy, const double theta);
    static bool XrayEscape(Photon &p, const GammaStats & mat_gamma_prop,
                           double & deposit);
    static void RayleighScatter(Photon &p);
    static constexpr double speed_of_light_cmpers = 29979245800.0;
    static constexpr double inverse_speed_of_light = (1.0 / speed_of_light_cmpers);
    static constexpr double energy_511 = 0.510998903;
    static constexpr double decays_per_microcurie = 37.0e3;

private:
    static double RayleighProbability(double theta);
    static double RayleighAngle();
};

#endif // PHYSICS_H
