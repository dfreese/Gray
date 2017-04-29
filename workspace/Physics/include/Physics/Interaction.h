#ifndef CSE167_INTERACTION_H
#define CSE167_INTERACTION_H

#include <ostream>
#include <iostream>
#include <vector>
#include <Physics/Photon.h>

class GammaStats;
class NuclearDecay;

class Interaction {
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
    int scatter_compton_phantom;
    int scatter_compton_detector;
    int scatter_rayleigh_phantom;
    int scatter_rayleigh_detector;
    int xray_flouresence;
    bool sensitive_mat;

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
    static void merge_interactions(Interaction & i0, const Interaction & i1);

    static int header_start_magic_number;
    static int output_version_number;
    static bool write_header(std::ostream & output, bool binary);
    static bool read_header(std::istream & input, bool & binary,
                            int & version);

    struct WriteFlags {
        bool time;
        bool id;
        bool color;
        bool type;
        bool pos;
        bool energy;
        bool det_id;
        bool src_id;
        bool mat_id;
        bool scatter_compton_phantom;
        bool scatter_compton_detector;
        bool scatter_rayleigh_phantom;
        bool scatter_rayleigh_detector;
        bool xray_flouresence;
        bool sensitive_mat;

        WriteFlags() :
            time(true),
            id(true),
            color(true),
            type(true),
            pos(true),
            energy(true),
            det_id(true),
            src_id(true),
            mat_id(true),
            scatter_compton_phantom(true),
            scatter_compton_detector(true),
            scatter_rayleigh_phantom(true),
            scatter_rayleigh_detector(true),
            xray_flouresence(true),
            sensitive_mat(true)
        {}
    };
    static void write_flags_stats(const WriteFlags & flags, int & no_fields,
                                  int & no_active);
    static int event_size(const WriteFlags & flags);
    static bool write_write_flags(const WriteFlags & flags,
                                  std::ostream & output, bool binary);
    static bool read_write_flags(WriteFlags & flags,
                                 std::istream & input, bool binary);
    static bool parse_write_flags_mask(WriteFlags & flags,
                                       const std::string & mask);
    static bool write_interaction(const Interaction & inter,
                                  std::ostream & output,
                                  const WriteFlags & flags, bool binary);
    static bool read_interaction(Interaction & inter,
                                 std::istream & input,
                                 const WriteFlags & flags, bool binary);

    static INTER_TYPE InteractionType(Photon &p,
                                      double & dist,
                                      const GammaStats & mat_gamma_prop,
                                      double & deposit);
    static void ComptonScatter(Photon &p, double & deposit);
    static void KleinNishinaAngle(double energy, double & theta,
                                    double & phi);
    static double KleinNishinaEnergy(double energy, double theta);
    static bool XrayEscape(Photon &p, const GammaStats & mat_gamma_prop,
                           double & deposit);
    static const double speed_of_light_cmpers;
    static const double inverse_speed_of_light;
    static void RayleighScatter(Photon &p);
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
