#ifndef CSE167_SOURCELIST_H
#define CSE167_SOURCELIST_H

#include <set>
#include <string>
#include <vector>

class Source;
class Isotope;
class VectorR3;

class SourceList
{
public:
    SourceList();
    ~SourceList();
    Source * Decay();
    void AddSource(Source * s);
    bool SetCurIsotope(const std::string & iso);
    double GetMeanTotalEvents(double time);
    double GetTotalEvents(double time);

private:
    static const int MAX_REJECT_COUNTER = 100000;
    double curTime;
    int search(double e, int b_idx, int s_idx);
    double total_activity;
    std::vector <Source*> list;
    std::vector <Source*> neg_list;
    std::vector <Isotope*> isotopes;
    std::vector <double> prob;
    double CalculateTime();
    double GetTime();
    bool Inside(const VectorR3 & pos);
    double mean_time_between_events;
    unsigned int photon_number;
    std::set<std::string> valid_isotopes;
    std::string current_isotope;
    // a microcurie is 37kevents/second
    const double microCurie = 37.0e3;
};

#endif
