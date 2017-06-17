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
#include <fstream>
#include <sstream>

using namespace std;

SourceList::SourceList() :
    decay_number(0),
    simulate_isotope_half_life(true),
    start_time(0)
{
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
        if (valid_positrons.count(current_isotope) > 0) {
            Positron * pos = new Positron(valid_positrons[current_isotope]);
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
    // Time advances even if the decay is rejected by the inside negative
    // source test.  This is by design, as we do not know how much activity a
    // negative source inherently removes from the positive sources.
    if (InsideNegative(decay_pos)) {
        return(NULL);
    } else {
        decay_number++;
        return(source);
    }
}

bool SourceList::InsideNegative(const VectorR3 & pos)
{
    for (int i = 0; i < neg_list.size(); i++) {
        if (neg_list[i]->Inside(pos)) {
            double ratio = -1 * neg_list[i]->GetActivity();
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

double SourceList::ExpectedDecays(double start_time, double sim_time) {
    double total = 0;
    for (Source * source: list) {
        double activity = source->GetActivity();
        double half_life = source->GetIsotope()->GetHalfLife();
        double source_decays = (pow(0.5, start_time / half_life) -
                                pow(0.5, (start_time + sim_time) / half_life)) *
                               (half_life / log(2.0)) * activity;
        if (!simulate_isotope_half_life) {
            source_decays = sim_time * activity;
        }
        total += source_decays;
    }
    return (total);
}

double SourceList::ExpectedPhotons(double start_time, double sim_time) {
    double total = 0;
    for (Source * source: list) {
        double activity = source->GetActivity();
        double half_life = source->GetIsotope()->GetHalfLife();
        double source_decays = (pow(0.5, start_time / half_life) -
                                pow(0.5, (start_time + sim_time) / half_life)) *
        (half_life / log(2.0)) * activity;
        if (!simulate_isotope_half_life) {
            source_decays = sim_time * activity;
        }
        total += source_decays * source->GetIsotope()->ExpectedNoPhotons();
    }
    return (total);
}

void SourceList::InitSources() {
    for (size_t sidx = 0; sidx < list.size(); sidx++) {
        AddNextDecay(sidx, start_time);
    }
}

bool SourceList::LoadIsotopes(const std::string &filename) {
    ifstream input(filename);

    if (!input) {
        cerr << filename << " not found." << endl;
        return(false);
    }

    string line;
    while (getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        line = line.substr(0, line.find_first_of("#"));
        // Ignore blank lines again after removing comments
        if (line.empty()) {
            continue;
        }
        string isotope_name;
        double acolinearity;
        double half_life;
        double pos_emission_prob;
        double gamma_emission_energy;
        string positron_model;

        stringstream line_ss(line);
        bool fail = false;
        fail |= (line_ss >> isotope_name).fail();
        fail |= (line_ss >> acolinearity).fail();
        fail |= (line_ss >> half_life).fail();
        fail |= (line_ss >> pos_emission_prob).fail();
        fail |= (line_ss >> gamma_emission_energy).fail();
        fail |= (line_ss >> positron_model).fail();
        if (fail) {
            return(false);
        }
        if (half_life <= 0) {
            half_life = std::numeric_limits<double>::infinity();
        }

        if ((pos_emission_prob < 0) || (pos_emission_prob > 1.0)) {
            pos_emission_prob = 1.0;
        }
        if (gamma_emission_energy <= 0) {
            valid_positrons[isotope_name] = Positron(acolinearity, half_life,
                                                     pos_emission_prob);
        } else {
            valid_positrons[isotope_name] = Positron(acolinearity, half_life,
                                                     pos_emission_prob,
                                                     gamma_emission_energy);
        }

        if (positron_model == "none") {
        } else if (positron_model == "gauss") {
            double fwhm_mm;
            double max_mm;
            fail |= (line_ss >> fwhm_mm).fail();
            fail |= (line_ss >> max_mm).fail();
            if (fail) {
                cerr << "Unable to parse gauss fwhm_mm or max_mm" << endl;
                return(false);
            }
            valid_positrons[isotope_name] = Positron(acolinearity, half_life,
                                                     pos_emission_prob,
                                                     gamma_emission_energy);
            valid_positrons[isotope_name].SetPositronRange(fwhm_mm, max_mm);
        } else if (positron_model == "levin_exp") {
            double c;
            double k1;
            double k2;
            double max_mm;
            fail |= (line_ss >> c).fail();
            fail |= (line_ss >> k1).fail();
            fail |= (line_ss >> k2).fail();
            fail |= (line_ss >> max_mm).fail();
            if (fail) {
                cerr << "Unable to parse levin_exp options" << endl;
                return(false);
            }
            valid_positrons[isotope_name] = Positron(acolinearity, half_life,
                                                     pos_emission_prob,
                                                     gamma_emission_energy);
            valid_positrons[isotope_name].SetPositronRange(c, k1, k2, max_mm);
        } else {
            cerr << "unrecognized positron model: " << positron_model << endl;
            return(false);
        }

        // Set the default isotope to the first one
        if (current_isotope == "") {
            current_isotope = isotope_name;
        }
    }
    input.close();
    return(true);
}
