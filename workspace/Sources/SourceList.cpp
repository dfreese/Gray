#include <SourceList.h>

using namespace std;

SourceList::SourceList()
{
    curTime = 0.0;
    photon_number = 0;
    curIsotope = ISO_BACKBACK;
}

// Garry: Added destructor for proper cleanup
SourceList::~SourceList()
{
    list.clear();
    neg_list.clear();
    prob.clear();
}

inline double SourceList::Random()
{
    return rand()/(double)RAND_MAX;
}

void SourceList::AddSource(Source & s)
{

    Source * src = &s;

    // TODO: Add the ability to change the isotopes from the DFF file
    src->SetIsotope(isotope.newIsotope(curIsotope));
    if (s.isNegative()) {
        cout << "Adding negative source\n";
        neg_list.push_back(src);
        src->SetSourceNum(-1*neg_list.size());
    } else {
        list.push_back(src);
        src->SetSourceNum(list.size());
        total_activity += s.GetActivity();
        mean_time_between_events = (1.0) / (total_activity * microCurie);
        prob.push_back(total_activity);
    }
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

Isotope * SourceList::Decay()
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
        idx = search(Random()*total_activity,0,s_idx);
        list[idx]->Reset();
        list[idx]->Decay(photon_number);
    } while ( Inside(list[idx]->GetDecayPosition()) && ( (counter++) < MAX_REJECT_COUNTER));
    if (counter == MAX_REJECT_COUNTER) {
        return NULL;
    } else {
        list[idx]->SetTime(GetTime());
        CalculateTime();
        photon_number++;
        return list[idx]->GetIsotope();
    }
}

double SourceList::CalculateTime()
{
    // performance speedup double mean_time_between_events = (1.0) / (total_activity * microCurie);
    double deltaT = -1.0 * log(1.0 - Random()) * mean_time_between_events;
    curTime += deltaT;
    return curTime;
}

double SourceList::GetTotalEvents(double time)
{
    // the number of events is the total time times the activity times the number of microcuries
    mean_time_between_events = (1.0) / (total_activity * microCurie);
    return time * total_activity * microCurie;
}

bool SourceList::Inside(const VectorR3 & pos)
{
    for (int i = 0; i < neg_list.size(); i++) {
        if (neg_list[i]->Inside(pos)) {
            double ratio = fabs((neg_list[i]->GetActivity()));
            ratio = ratio > 1.0 ? 1.0 : ratio;
            ratio = ratio < 0.0 ? 0.0 : ratio;
            if (Random() < ratio) {
                return true;
            }
        }
    }
    return false;
}

void SourceList::SetCurIsotope(const char * iso)
{
    curIsotope = isotope.getType(iso);
}
