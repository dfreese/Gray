/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef SOURCELIST_H
#define SOURCELIST_H

#include <iosfwd>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include "Gray/Physics/Positron.h"
#include "Gray/Sources/Source.h"
#include "Gray/json/json.h"

class Isotope;
class VectorR3;
class RigidMapR3;

class SourceList
{
public:
    SourceList() = default;
    NuclearDecay Decay();
    void AddSource(std::unique_ptr<Source> s);
    void AddIsotope(const std::string& name, std::unique_ptr<Isotope> s);
    bool SetCurIsotope(const std::string& iso, const RigidMapR3& cur_matrix);
    void SetSimulationTime(double time);
    double GetTime() const;
    double GetElapsedTime() const;
    double GetSimulationTime() const;
    double GetEndTime() const;
    bool SimulationIncomplete() const;
    std::vector<VectorR3> GetSourcePositions() const;
    void DisableHalfLife();
    void SetStartTime(double val);
    void InitSources();
    bool LoadIsotopes(const std::string& physics_filename);
    void AdjustTimeForSplit(int idx, int n);
    bool PrintSplits(int n) const;
    double ExpectedPhotons(double start_time, double sim_time) const;
    void CalculateEqualPhotonTimeSplits(
        double start_time, double full_sim_time, int n,
        std::vector<double> & split_start,
        std::vector<double> & split_length) const;
    double SearchSplitTime(double start_time, double full_sim_time,
                           double split_start, double no_photons) const;
    size_t NumSources() const;
    std::shared_ptr<const Source> GetSource(size_t idx) const;
    static std::unique_ptr<Isotope> IsotopeFactory(
            Json::Value isotope, bool simulate_isotope_half_life);
    bool LoadIsotopes(std::istream& input);
private:
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
    std::map<std::string, std::shared_ptr<const Isotope>> valid_isotopes;
    int decay_number = 0;
    std::string current_isotope;
    double simulation_time = 0;

    // Hold all of the information on the upcoming decays in a min-priority
    // queue, so the earliest time event is in front.
    std::priority_queue<DecayInfo, std::vector<DecayInfo>,
            std::greater<DecayInfo>> decay_list;
    bool simulate_isotope_half_life = true;
    double start_time = 0;
    double end_time = 0;
};

#endif
