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
    double GetMu(double e) const;
    void GetInteractionProbs(double e, double & pe, double & comp,
                             double & ray) const;
    void AddEscape(double * xray_escapes, double * xray_probs, double * auger_probs, int num);
    bool GetEscape(double rand, const double photon_energy, double &xray_energy) const;
    int GetNumEscape() const;
    double * GetXrayEscape() const;
    double * GetXrayEscapeProb() const;
    double GetAugerProb(int i) const;
    bool log_material;
    bool enable_interactions;
    void DisableInteractions();
    std::string GetName() const;

private:

    int search(double e, int b_idx, int s_idx) const;

    std::string name;
    std::string filename;
    std::vector<double> energy;
    std::vector<double> mu;
    std::vector<double> photoelectric;
    std::vector<double> compton;
    std::vector<double> rayleigh;
    double * xray_escape;
    double * xray_escape_probability;
    double * auger;

    // cache for energy lookup
    double cache_energy;
    int cache_idx;

    int size;
    int num_escape;
    int material;
};

#endif
