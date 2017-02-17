#include <Sources/SourceList.h>
#include <Random/Random.h>
#include <Physics/F18.h>
#include <Physics/IN110.h>
#include <Physics/ZR89.h>
#include <Physics/BackBack.h>
#include <Physics/Beam.h>

using namespace std;

SourceList::SourceList()
{
    curTime = 0.0;
    total_activity = 0.0;
    photon_number = 0;
    current_isotope = "BackBack";
    valid_isotopes.insert("F18");
    valid_isotopes.insert("IN110");
    valid_isotopes.insert("ZR89");
    valid_isotopes.insert("BackBack");
    valid_isotopes.insert("Beam");

}

// Garry: Added destructor for proper cleanup
SourceList::~SourceList()
{
    list.clear();
    neg_list.clear();
    prob.clear();
}

void SourceList::AddSource(Source * s)
{
    Isotope * isotope;
    if (current_isotope == "F18") {
        isotope = static_cast<Isotope *>(new F18());
    } else if (current_isotope == "IN110") {
        isotope = static_cast<Isotope *>(new IN110());
    } else if (current_isotope == "ZR89") {
        isotope = static_cast<Isotope *>(new ZR89());
    } else if (current_isotope == "BackBack") {
        isotope = static_cast<Isotope *>(new BackBack());
    } else if (current_isotope == "Beam") {
        isotope = static_cast<Isotope *>(new Beam());
    } else {
        string error = "Isotope named " + current_isotope
        + " passed valid_isotope test, but was not implemented";
        throw(runtime_error(error));
    }
    
    s->SetIsotope(isotope);
    if (s->isNegative()) {
        cout << "Adding negative source\n";
        neg_list.push_back(s);
        s->SetSourceNum(-1*neg_list.size());
    } else {
        list.push_back(s);
        s->SetSourceNum(list.size());
        total_activity += s->GetActivity();
        mean_time_between_events = (1.0) / (total_activity * microCurie);
        prob.push_back(total_activity);
    }
}

double SourceList::GetTime()
{
    return curTime;
}

int SourceList::search(double e, int b_idx, int s_idx)
{
    if (b_idx == s_idx) {
        return b_idx;
    }
    int idx = (int)(((b_idx) + (s_idx))/2);
    if (prob[idx] < e) {
        return search(e,idx+1,s_idx);
    } else {
        return search(e,b_idx,idx);
    }
}

Source * SourceList::Decay()
{
    if (prob.empty()) {
        cerr << "ERROR: Sources do not exit\n";
        exit(0);
    }
    int s_idx = prob.size()-1;
    int counter = 0;

    // FIXME Event ID is hosed
    unsigned int idx = 0;
    do {
        idx = search(Random::Uniform()*total_activity,0,s_idx);
        list[idx]->Reset();
        list[idx]->Decay(photon_number);
    } while ( Inside(list[idx]->GetDecayPosition()) && ( (counter++) < MAX_REJECT_COUNTER));
    if (counter == MAX_REJECT_COUNTER) {
        return NULL;
    } else {
        list[idx]->SetTime(GetTime());
        CalculateTime();
        photon_number++;
        return list[idx];
    }
}

double SourceList::CalculateTime()
{
    // performance speedup double mean_time_between_events = (1.0) / (total_activity * microCurie);
    double deltaT = -1.0 * log(1.0 - Random::Uniform()) * mean_time_between_events;
    curTime += deltaT;
    return curTime;
}

double SourceList::GetMeanTotalEvents(double time)
{
    // the number of events is the total time times the activity times the number of microcuries
    mean_time_between_events = (1.0) / (total_activity * microCurie);
    return time * total_activity * microCurie;
}

double SourceList::GetTotalEvents(double time)
{
    return(Random::Poisson(GetMeanTotalEvents(time)));
}

bool SourceList::Inside(const VectorR3 & pos)
{
    for (int i = 0; i < neg_list.size(); i++) {
        if (neg_list[i]->Inside(pos)) {
            double ratio = fabs((neg_list[i]->GetActivity()));
            ratio = ratio > 1.0 ? 1.0 : ratio;
            ratio = ratio < 0.0 ? 0.0 : ratio;
            if (Random::Uniform() < ratio) {
                return true;
            }
        }
    }
    return false;
}

bool SourceList::SetCurIsotope(const std::string & iso)
{
    if (valid_isotopes.count(iso)) {
        current_isotope = iso;
        return(true);
    } else {
        return(false);
    }

}
