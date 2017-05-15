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
    void SetSize(int s);
    void SetMaterialType(int s);
    bool Load();
    int GetIndex(double e) const;
    int GetMaterial() const;
    void GetInteractionProbs(double e, double & pe, double & comp,
                             double & ray) const;
    bool log_material;
    bool enable_interactions;
    void DisableInteractions();
    std::string GetName() const;
    const std::vector<double> & GetXrayEmissionEnergies() const;
    const std::vector<double> & GetXrayEmissionCumProb() const;

    double GetXrayBindEnergyScale(double energy) const;

private:

    int search(double e, int b_idx, int s_idx) const;

    std::string name;
    std::string filename;
    std::vector<double> energy;
    std::vector<double> photoelectric;
    std::vector<double> compton;
    std::vector<double> rayleigh;
    std::vector<double> xray_binding_energy;
    std::vector<double> xray_emission_energy;
    std::vector<double> xray_emission_prob;
    std::vector<double> xray_emission_cumprob;

    std::vector<double> unique_xray_binding_energy;
    std::vector<double> xray_binding_enery_scale;

    // cache for energy lookup
    double cache_energy;
    int cache_idx;

    int size;
    int num_escape;
    int material;
};

#endif
