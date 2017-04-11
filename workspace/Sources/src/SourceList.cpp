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
#include <exception>

using namespace std;

SourceList::SourceList()
{
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
        if (s->GetActivity() < -1.0) {
            string error = "Negative Source activities should be [-1,0)";
            throw(runtime_error(error));
        }
        cout << "Adding negative source\n";
        neg_list.push_back(s);
        s->SetSourceNum(-1 * static_cast<int>(neg_list.size()));
    } else {
        s->SetSourceNum(static_cast<int>(list.size()));
        list.push_back(s);
        AddNextDecay(list.size() - 1, 0);
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

double SourceList::GetTime() const
{
    if (decay_list.empty()) {
        // If there are no new decays, just assume we're at the end of the
        // simulation.  This should only happen if no sources were added to
        // start off with.
        return(GetSimulationTime());
    }
    // Set the current time to be the next decay that will happen.  This won't
    // be accessed until the next iteration of the main loop, this way we don't
    // simulate events outside of the simulation time.
    return((*decay_list.begin()).first);
}

double SourceList::GetSimulationTime() const
{
    return(simulation_time);
}

void SourceList::AddNextDecay(size_t source_idx, double base_time) {
    // Calculating the next source decay time
    Source * source = list[source_idx];
    double decay_time = Random::Exponential(source->GetActivity() * microCurie);
    decay_list[base_time + decay_time] = source_idx;
}

void SourceList::GetNextDecay(size_t & source_idx, double & time) {
    time = (*decay_list.begin()).first;
    source_idx = (*decay_list.begin()).second;
    decay_list.erase(decay_list.begin());
}

Source * SourceList::Decay()
{
    if (list.empty()) {
        string error = "Decay called with no sources to decay";
        throw(runtime_error(error));
    }

    double decay_time;
    size_t s_idx;
    GetNextDecay(s_idx, decay_time);
    AddNextDecay(s_idx, decay_time);
    Source * source = list[s_idx];
    source->Reset();
    VectorR3 decay_pos = source->Decay(decay_number, decay_time);
    if (Inside(decay_pos)) {
        return(NULL);
    } else {
        decay_number++;
        return(source);
    }
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
