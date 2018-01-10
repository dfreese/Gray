#ifndef CSE167_SOURCELIST_H
#define CSE167_SOURCELIST_H

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <Physics/Positron.h>
#include <Sources/Source.h>
#include <json/json.h>

class Isotope;
class VectorR3;
class RigidMapR3;

class SourceList
{
public:
    SourceList();
    NuclearDecay Decay();
    void AddSource(std::unique_ptr<Source> s);
    bool SetCurIsotope(const std::string& iso, const RigidMapR3& cur_matrix);
    void SetSimulationTime(double time);
    double GetTime() const;
    double GetElapsedTime() const;
    double GetSimulationTime() const;
    double GetEndTime() const;
    bool SimulationIncomplete() const;
    std::vector<VectorR3> GetSourcePositions() const;
    void SetSimulateIsotopeHalfLife(bool val);
    void SetStartTime(double val);
    void InitSources();
    bool LoadIsotopes(const std::string& physics_filename);
    void AdjustTimeForSplit(int idx, int n);
    void PrintSplits(int n) const;

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
    bool CreateBeamIsotope(const std::string & iso,
                           const RigidMapR3& current_matrix);
    bool InsideNegative(const VectorR3 & pos) const;
    bool LoadIsotope(const std::string& iso_name, Json::Value isotope);

    struct DecayInfo {
        double time;
        int source_idx;
        VectorR3 position;
        bool operator<(const DecayInfo & rhs) const {
            return (time < rhs.time);
        }
        bool operator>(const DecayInfo & rhs) const {
            return (time > rhs.time);
        }
    };
    DecayInfo NextDecay(DecayInfo base_info) const;
    DecayInfo GetNextDecay();

    // We keep these as shared_ptrs as we assume this SourceList can be copied
    // and used by different threads.  The access to the sources and isotopes
    // is entirely read-only and const, so it is fine to be accessed across
    // different threads.  That however, means care must be taken to make sure
    // it stays that way.
    std::vector<std::shared_ptr<const Source>> list;
    std::vector<std::shared_ptr<const Source>> neg_list;
    std::map<std::string, std::shared_ptr<Isotope>> valid_isotopes;
    int decay_number;
    std::string current_isotope;
    double simulation_time;

    // Hold all of the information on the upcoming decays in a min-priority
    // queue, so the earliest time event is in front.
    std::priority_queue<DecayInfo, std::vector<DecayInfo>,
            std::greater<DecayInfo>> decay_list;
    bool simulate_isotope_half_life;
    double start_time;
    double end_time;
};

#endif
