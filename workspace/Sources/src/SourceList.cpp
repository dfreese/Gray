#include <Sources/SourceList.h>
#include <Random/Random.h>
#include <Physics/F18.h>
#include <Physics/IN110.h>
#include <Physics/ZR89.h>
#include <Physics/BackBack.h>
#include <Physics/Beam.h>
#include <Sources/Source.h>
#include <Sources/BeamPointSource.h>
#include <Sources/VectorSource.h>
#include <VrMath/LinearR3.h>

using namespace std;

SourceList::SourceList()
{
    curTime = 0.0;
    total_activity = 0.0;
    decay_number = 0;
    acolinearity = PositronDecay::default_acolinearity;
    current_isotope = "BackBack";
    valid_isotopes.insert("F18");
    valid_isotopes.insert("IN110");
    valid_isotopes.insert("ZR89");
    valid_isotopes.insert("BackBack");
    valid_isotopes.insert("Beam");

}

SourceList::~SourceList()
{
    while (!list.empty()) {
        delete list.back();
        list.pop_back();
    }
    while (!neg_list.empty()) {
        delete neg_list.back();
        neg_list.pop_back();
    }
    while (!isotopes.empty()) {
        delete isotopes.back();
        isotopes.pop_back();
    }
}

void SourceList::AddSource(Source * s)
{
    Isotope * isotope;

    // BeamPointSource requires a beam isotope, so override the current isotope
    // setting to make sure that is given.
    BeamPointSource * beam_pt_src = dynamic_cast<BeamPointSource *>(s);
    if (beam_pt_src) {
        isotope = static_cast<Isotope *>(new Beam());
    } else {
        if (current_isotope == "F18") {
            isotope = static_cast<Isotope *>(new F18(acolinearity));
        } else if (current_isotope == "IN110") {
            isotope = static_cast<Isotope *>(new IN110(acolinearity));
        } else if (current_isotope == "ZR89") {
            isotope = static_cast<Isotope *>(new ZR89(acolinearity));
        } else if (current_isotope == "BackBack") {
            isotope = static_cast<Isotope *>(new BackBack(acolinearity));
        } else if (current_isotope == "Beam") {
            isotope = static_cast<Isotope *>(new Beam());
        } else {
            string error = "Isotope named " + current_isotope
            + " passed valid_isotope test, but was not implemented";
            throw(runtime_error(error));
        }
    }
    isotopes.push_back(isotope);
    
    s->SetIsotope(isotope);
    if (s->isNegative()) {
        cout << "Adding negative source\n";
        neg_list.push_back(s);
        s->SetSourceNum(-1 * static_cast<int>(neg_list.size()));
    } else {
        s->SetSourceNum(static_cast<int>(list.size()));
        list.push_back(s);
        total_activity += s->GetActivity();
        mean_time_between_events = (1.0) / (total_activity * microCurie);
        prob.push_back(total_activity);
    }
}

void SourceList::SetKdTree(IntersectKdTree & tree) {
    // We add the KdTree to the sources later, as the tree is not constructed
    // until after all of the sources are made, and the geometry is determined.
    // Probably could pass along a pointer initially into LoadDetector, but
    // this also works for now.
    for (auto source: list) {
        VectorSource * vec_src = dynamic_cast<VectorSource *>(source);
        if (vec_src) {
            vec_src->SetKdTree(tree);
        }
    }
    // A vector source cannot be negative, so we do not check the neg_list
}

double SourceList::GetTime()
{
    return curTime;
}

size_t SourceList::search(double e, size_t b_idx, size_t s_idx)
{
    if (b_idx == s_idx) {
        return b_idx;
    }
    size_t idx = (int)(((b_idx) + (s_idx))/2);
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
    size_t s_idx = prob.size() - 1;
    int counter = 0;

    // FIXME Event ID is hosed
    size_t idx = 0;
    VectorR3 decay_pos;
    do {
        idx = search(Random::Uniform()*total_activity, 0, s_idx);
        list[idx]->Reset();
        decay_pos = list[idx]->Decay(decay_number, curTime);
    } while (Inside(decay_pos) && ( (counter++) < MAX_REJECT_COUNTER));
    if (counter == MAX_REJECT_COUNTER) {
        return NULL;
    } else {
        CalculateTime();
        decay_number++;
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


double SourceList::GetMeanTotalEvents()
{
    return(GetMeanTotalEvents(simulation_time));
}

double SourceList::GetTotalEvents()
{
    return(GetTotalEvents(simulation_time));
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

void SourceList::SetAcolinearity(double acolinearity_deg_fwhm)
{
    if (acolinearity_deg_fwhm < 0) {
        acolinearity = PositronDecay::default_acolinearity;
    } else {
        acolinearity = acolinearity_deg_fwhm;
    }
}

void SourceList::SetSimulationTime(double time)
{
    simulation_time = time;
}
