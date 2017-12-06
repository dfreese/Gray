#ifndef GAMMA_STATS_H
#define GAMMA_STATS_H

#include <string>
#include <vector>

class GammaStats
{
public:

    GammaStats();
    GammaStats(const std::string & name, int index,
               double density, bool sensitive, std::vector<double> energy,
               std::vector<double> matten_comp, std::vector<double> matten_phot,
               std::vector<double> matten_rayl, std::vector<double> x,
               std::vector<double> form_factor,
               std::vector<double> scattering_func);
    void SetName(const std::string & n);
    void SetFileName(const std::string & n);
    void SetMaterialType(int s);
    bool Load();
    int GetMaterial() const;
    void GetInteractionProbs(double e, double & pe, double & comp,
                             double & ray) const;
    bool LogMaterial() const {
        return (log_material);
    }
    void SetLogMaterial(bool val) {
        log_material = val;
    }
    bool InteractionsEnabled() const;
    void DisableInteractions();
    std::string GetName() const;
    double GetComptonScatterAngle(double energy) const;

    // A class for static initialization of the dsigma_max values as a function
    // of energy
    class KleinNishina {
    public:
        KleinNishina();
        static double dsigma(const double costheta, const double energy_mev);
        static std::vector<std::vector<double>> create_scatter_cdfs(
                const std::vector<double> & energies,
                const std::vector<double> & thetas);
        double scatter_angle(double energy, double rand_uniform) const;
    private:
        std::vector<double> energy_idx;
        std::vector<double> costheta_idx;
        std::vector<std::vector<double>> scatter_cdfs;
    };

private:
    size_t GetIndex(double e) const;
    std::string name;
    int index;
    std::string filename;
    std::vector<double> energy;
    std::vector<double> photoelectric;
    std::vector<double> compton;
    std::vector<double> rayleigh;
    std::vector<double> log_energy;
    std::vector<double> log_photoelectric;
    std::vector<double> log_compton;
    std::vector<double> log_rayleigh;
    const std::vector<double> x;
    const std::vector<double> form_factor;
    const std::vector<double> scattering_func;

    bool enable_interactions;
    bool log_material;

    KleinNishina klein_nishina;

    // cache for energy lookup
    mutable double cache_energy_min;
    mutable double cache_energy_max;
    mutable size_t cache_idx;
};

#endif
