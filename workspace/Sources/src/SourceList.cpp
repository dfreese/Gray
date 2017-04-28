#include <Sources/SourceList.h>
#include <Random/Random.h>
#include <Physics/Beam.h>
#include <Physics/Positron.h>
#include <Physics/PositronDecay.h>
#include <Sources/Source.h>
#include <Sources/BeamPointSource.h>
#include <Sources/VectorSource.h>
#include <VrMath/LinearR3.h>
#include <exception>
#include <limits>

using namespace std;

SourceList::SourceList() :
    decay_number(0),
    current_isotope("BackBack"),
    acolinearity(PositronDecay::default_acolinearity),
    simulate_isotope_half_life(true),
    start_time(0)
{
    // TODO: read these isotopes and their properties in from a file
    const double inf = std::numeric_limits<double>::infinity();
    valid_positrons["BackBack"] = Positron(acolinearity, inf);
    valid_positrons["F18"] = Positron(acolinearity, 6584.04, 0.9686);
    valid_positrons["F18"].SetPositronRange(0.519, 27.9, 2.91, 3.0);

    valid_positrons["O15"] = Positron(acolinearity, 122.46, 0.99885);
    valid_positrons["O15"].SetPositronRange(0.263, 33.2, 1.0, 3.0);

    valid_positrons["N13"] = Positron(acolinearity, 598.02, 0.99818);
    valid_positrons["N13"].SetPositronRange(0.433, 25.4, 1.44, 3.0);

    valid_positrons["C11"] = Positron(acolinearity, 1221.66, 0.9975);
    valid_positrons["C11"].SetPositronRange(0.501, 24.5, 1.76, 3.0);

    // FIXME: stop using F18 positron range for In110 and Zr89.
    valid_positrons["In110"] = Positron(acolinearity, 17676.0, 0.61, 0.657750);
    valid_positrons["In110"].SetPositronRange(0.519, 27.9, 2.91, 3.0);

    valid_positrons["Zr89"] = Positron(acolinearity, 282280.32, 0.227, 0.90915);
    valid_positrons["Zr89"].SetPositronRange(0.519, 27.9, 2.91, 3.0);
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
        if (valid_positrons.count(current_isotope)) {
            Positron * pos = new Positron(valid_positrons[current_isotope]);
            pos->set_acolinearity(acolinearity);
            isotope = static_cast<Isotope *>(pos);
        } else {
            string error = "Isotope named " + current_isotope
                + " somehow set as current isotope, but was not implemented";
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
        neg_list.push_back(s);
        s->SetSourceNum(-1 * static_cast<int>(neg_list.size()));
    } else {
        s->SetSourceNum(static_cast<int>(list.size()));
        list.push_back(s);
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
        // If there are no new decays, assume there were no sources and return
        // the end of the simulation
        return(simulation_time + start_time);
    }
    // Set the current time to be the next decay that will happen.  This won't
    // be accessed until the next iteration of the main loop, this way we don't
    // simulate events outside of the simulation time.
    return((*decay_list.begin()).first);
}

double SourceList::GetElapsedTime() const {
    return(GetTime() - start_time);
}

double SourceList::GetSimulationTime() const
{
    return(simulation_time);
}

double SourceList::GetEndTime() const {
    return(simulation_time + start_time);
}

void SourceList::AddNextDecay(size_t source_idx, double base_time) {
    // Calculating the next source decay time
    Source * source = list[source_idx];
    double source_activity_bq = source->GetActivity() * microCurie;
    if (simulate_isotope_half_life) {
        Isotope * isotope = source->GetIsotope();
        source_activity_bq *= isotope->FractionRemaining(base_time);
    }
    double decay_time = Random::Exponential(source_activity_bq);
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
    if (valid_positrons.count(iso)) {
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

void SourceList::SetSimulateIsotopeHalfLife(bool val) {
    simulate_isotope_half_life = val;
}

void SourceList::SetStartTime(double val)
{
    start_time = val;
}

void SourceList::InitSources() {
    for (size_t sidx = 0; sidx < list.size(); sidx++) {
        AddNextDecay(sidx, start_time);
    }
}

