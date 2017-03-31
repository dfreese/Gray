#ifndef CSE167_SOURCELIST_H
#define CSE167_SOURCELIST_H

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
    double GetMeanTotalEvents(double time);
    double GetTotalEvents(double time);
    double GetMeanTotalEvents();
    double GetTotalEvents();
    void SetKdTree(IntersectKdTree & tree);
    void SetSimulationTime(double time);

private:
    static const int MAX_REJECT_COUNTER = 100000;
    double curTime;
    size_t search(double e, size_t b_idx, size_t s_idx);
    double total_activity;
    std::vector <Source*> list;
    std::vector <Source*> neg_list;
    std::vector <Isotope*> isotopes;
    std::vector <double> prob;
    double CalculateTime();
    double GetTime();
    bool Inside(const VectorR3 & pos);
    double mean_time_between_events;
    int decay_number;
    std::set<std::string> valid_isotopes;
    std::string current_isotope;
    // a microcurie is 37kevents/second
    const double microCurie = 37.0e3;
    double simulation_time;
    double acolinearity;
};

#endif
