#ifndef GAMMA_STATS_H
#define GAMMA_STATS_H

class GammaStats
{
public:

    GammaStats();
    ~GammaStats();
    void SetName(char * n);
    void SetFileName(char * n);
    void SetSize(int s);
    void SetMaterialType(int s);
    bool Load(void);
    int GetIndex(double e);

    int GetMaterial(void)
    {
        return material;
    }
    double GetSigma(double e, int idx);
    double GetMu(double e, int idx);
    double GetTau(double e, int idx);
    double GetDsDom(double e);
    void GetPE(double e, double &m, double &s);
    void AddEscape(double * xray_escapes, double * xray_probs, double * auger_probs, int num);
    bool GetEscape(double rand, const double photon_energy, double &xray_energy);
    int GetNumEscape();
    double * GetXrayEscape();
    double * GetXrayEscapeProb();
    double GetAugerProb(int i);
    bool log_material;
    bool enable_interactions;
    void DisableInteractions();
    char * GetName()
    {
        return name;
    }

private:

    int search(double e, int b_idx, int s_idx);

    char * name;
    char * filename;
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

inline void GammaStats::DisableInteractions()
{
    enable_interactions = false;
}

inline int GammaStats::GetNumEscape()
{
    return num_escape;
}
inline double * GammaStats::GetXrayEscape()
{
    return xray_escape;
}
inline double * GammaStats::GetXrayEscapeProb()
{
    return xray_escape_probability;
}
inline double GammaStats::GetAugerProb(int i)
{
    return auger[i];
}

#endif
