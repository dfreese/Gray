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
    double GetTime() const;
    double GetSimulationTime() const;

private:
    static const int MAX_REJECT_COUNTER = 100000;
    double curTime;
    double total_activity;
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
};

#endif
