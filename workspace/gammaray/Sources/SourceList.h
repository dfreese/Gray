#ifndef CSE167_SOURCELIST_H
#define CSE167_SOURCELIST_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <Physics/Positron.h>
#include <Sources/Source.h>

class Isotope;
class VectorR3;
class SceneDescription;
class GammaMaterial;

class SourceList
{
public:
    SourceList();
    Source * Decay();
    void AddSource(std::unique_ptr<Source> s);
    bool SetCurIsotope(const std::string & iso);
    void SetSimulationTime(double time);
    double GetTime() const;
    double GetElapsedTime() const;
    double GetSimulationTime() const;
    double GetEndTime() const;
    void SetSimulateIsotopeHalfLife(bool val);
    void SetStartTime(double val);
    void InitSources();
    bool LoadIsotopes(const std::string & filename);
    void AdjustTimeForSplit(int idx, int n);
    void BuildMaterialStacks(const SceneDescription & scene,
                             GammaMaterial const * default_material);

private:
    double ExpectedDecays(double start_time, double sim_time) const;
    double ExpectedPhotons(double start_time, double sim_time) const;
    void CalculateEqualPhotonTimeSplits(
        double start_time, double full_sim_time, int n,
        std::vector<double> & split_start,
        std::vector<double> & split_length) const;
    double SearchSplitTime(double start_time, double full_sim_time,
                           double split_start, double no_photons,
                           double tol) const;
    std::vector <std::unique_ptr<Source>> list;
    std::vector <std::unique_ptr<Source>> neg_list;
    double CalculateTime();
    bool InsideNegative(const VectorR3 & pos);
    int decay_number;
    std::map<std::string, Positron> valid_positrons;
    std::string current_isotope;
    // a microcurie is 37kevents/second
    const double microCurie = 37.0e3;
    double simulation_time;
    std::map<double, size_t> decay_list;
    void AddNextDecay(size_t source_idx, double base_time);
    void GetNextDecay(size_t & source_idx, double & time);
    bool simulate_isotope_half_life;
    double start_time;
};

#endif
