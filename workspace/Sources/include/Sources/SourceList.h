#ifndef CSE167_SOURCELIST_H
#define CSE167_SOURCELIST_H

#include <map>
#include <set>
#include <string>
#include <vector>

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
    double GetTime();
    double GetSimulationTime() const;
    void SetSimulateIsotopeHalfLife(bool val);
    void SetStartTime(double val);

private:
    std::vector <Source*> list;
    std::vector <Source*> neg_list;
    std::vector <Isotope*> isotopes;
    double CalculateTime();
    bool Inside(const VectorR3 & pos);
    int decay_number;
    std::set<std::string> valid_isotopes;
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
