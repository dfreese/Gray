#ifndef GAMMA_STATS_H
#define GAMMA_STATS_H

#include <string>

class GammaStats
{
public:

    GammaStats();
    ~GammaStats();
    void SetName(const std::string & n);
    void SetFileName(const std::string & n);
    void SetSize(int s);
    void SetMaterialType(int s);
    bool Load(void);
    int GetIndex(double e) const;
    int GetMaterial() const;
    double GetSigma(double e, int idx) const;
    double GetMu(double e, int idx) const;
    double GetTau(double e, int idx) const;
    double GetDsDom(double e) const;
    void GetPE(double e, double &m, double &s) const;
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
    double * energy;
    double * mu;
    double * sigma;
    double * tau;
    double * dsdom;
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
