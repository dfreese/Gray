#ifndef GAMMA_STATS_H
#define GAMMA_STATS_H

#include <string>
#include <vector>

class GammaStats
{
public:

    GammaStats();
    void SetName(const std::string & n);
    void SetFileName(const std::string & n);
    void SetMaterialType(int s);
    bool Load();
    int GetMaterial() const;
    void GetInteractionProbs(double e, double & pe, double & comp,
                             double & ray) const;
    bool log_material;
    bool InteractionsEnabled() const;
    void DisableInteractions();
    std::string GetName() const;
    const std::vector<double> & GetXrayEmissionEnergies() const;
    const std::vector<double> & GetXrayEmissionCumProb() const;

    double GetXrayBindEnergyScale(double energy) const;

private:
    size_t GetIndex(double e) const;
    std::string name;
    std::string filename;
    std::vector<double> energy;
    std::vector<double> photoelectric;
    std::vector<double> compton;
    std::vector<double> rayleigh;
    std::vector<double> log_energy;
    std::vector<double> log_photoelectric;
    std::vector<double> log_compton;
    std::vector<double> log_rayleigh;
    std::vector<double> xray_binding_energy;
    std::vector<double> xray_emission_energy;
    std::vector<double> xray_emission_prob;
    std::vector<double> xray_emission_cumprob;

    std::vector<double> unique_xray_binding_energy;
    std::vector<double> xray_binding_enery_scale;

    // cache for energy lookup
    mutable double cache_energy_min;
    mutable double cache_energy_max;
    mutable size_t cache_idx;

    int num_escape;
    int material;

    bool enable_interactions;
};

#endif
