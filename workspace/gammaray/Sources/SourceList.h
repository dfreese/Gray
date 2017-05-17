#ifndef CSE167_SOURCELIST_H
#define CSE167_SOURCELIST_H

#include <map>
#include <string>
#include <vector>
#include <Physics/Positron.h>

class Source;
class Isotope;
class VectorR3;
class IntersectKdTree;

class SourceList
{
public:
    SourceList();
    ~SourceList();
    Source * Decay();
    void AddSource(Source * s);
    bool SetCurIsotope(const std::string & iso);
    void SetAcolinearity(double acolinearity_deg_fwhm);
    void SetKdTree(IntersectKdTree & tree);
    void SetSimulationTime(double time);
    double GetTime() const;
    double GetElapsedTime() const;
    double GetSimulationTime() const;
    double GetEndTime() const;
    void SetSimulateIsotopeHalfLife(bool val);
    void SetStartTime(double val);
    void InitSources();
    bool LoadIsotopes(const std::string & filename);

private:
    std::vector <Source*> list;
    std::vector <Source*> neg_list;
    std::vector <Isotope*> isotopes;
    double CalculateTime();
    bool InsideNegative(const VectorR3 & pos);
    int decay_number;
    std::map<std::string, Positron> valid_positrons;
    std::string current_isotope;
    // a microcurie is 37kevents/second
    const double microCurie = 37.0e3;
    double simulation_time;
    double acolinearity;
    std::map<double, size_t> decay_list;
    void AddNextDecay(size_t source_idx, double base_time);
    void GetNextDecay(size_t & source_idx, double & time);
    bool simulate_isotope_half_life;
    double start_time;
};

#endif
