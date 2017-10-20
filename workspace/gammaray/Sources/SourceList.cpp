#include <Sources/SourceList.h>
#include <Random/Random.h>
#include <Physics/Beam.h>
#include <Physics/Positron.h>
#include <Physics/Physics.h>
#include <Sources/BeamPointSource.h>
#include <Sources/VectorSource.h>
#include <Graphics/VisiblePoint.h>
#include <Graphics/SceneDescription.h>
#include <Gray/GammaMaterial.h>
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

void SourceList::AddSource(std::unique_ptr<Source> s)
{
    std::unique_ptr<Isotope> isotope;
    // BeamPointSource requires a beam isotope, so override the current isotope
    // setting to make sure that is given.
    BeamPointSource * beam_pt_src = dynamic_cast<BeamPointSource *>(s.get());
    if (beam_pt_src) {
        isotope = std::unique_ptr<Isotope>(new Beam());
    } else {
        if (valid_positrons.count(current_isotope) > 0) {
            isotope = std::unique_ptr<Isotope>(
                    new Positron(valid_positrons[current_isotope]));
        } else {
            string error = "Isotope named " + current_isotope
                + " somehow set as current isotope, but was not implemented";
            throw(runtime_error(error));
        }
    }
    
    s->SetIsotope(std::move(isotope));
    if (s->isNegative()) {
        if (s->GetActivity() < -1.0) {
            string error = "Negative Source activities should be [-1,0)";
            throw(runtime_error(error));
        }
        s->SetSourceNum(static_cast<int>(neg_list.size()) - 1);
        neg_list.push_back(std::move(s));
    } else {
        s->SetSourceNum(static_cast<int>(list.size()));
        list.push_back(std::move(s));
    }
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
    return((*decay_list.begin()).time);
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

void SourceList::AddNextDecay(DecayInfo base_info) {
    // Calculating the next source decay timesize_t source_idx, double base_time
    auto & source = list[base_info.source_idx];
    double source_activity_bq = source->GetActivity();
    do {
        if (simulate_isotope_half_life) {
            Isotope & isotope = *source->GetIsotope();
            source_activity_bq *= isotope.FractionRemaining(base_info.time);
        }
        // Time advances even if the decay is rejected by the inside negative
        // source test.  This is by design, as we do not know how much activity
        // a negative source inherently removes from the positive sources.
        base_info.time += Random::Exponential(source_activity_bq);
        base_info.position = source->Decay();
    } while (InsideNegative(base_info.position));
    base_info.decay_number = decay_number++;
    decay_list.insert(base_info);
}

SourceList::DecayInfo SourceList::GetNextDecay() {
    DecayInfo ret_val(*decay_list.begin());
    decay_list.erase(decay_list.begin());
    return (ret_val);
}

NuclearDecay SourceList::Decay() {
    if (list.empty()) {
        string error = "Decay called with no sources to decay";
        throw(runtime_error(error));
    }

    DecayInfo decay = GetNextDecay();
    AddNextDecay(decay);
    auto & source = list[decay.source_idx];
    Isotope & isotope = *source->GetIsotope();
    return (isotope.Decay(decay.decay_number, decay.time, decay.source_idx,
                          decay.position));
}

bool SourceList::InsideNegative(const VectorR3 & pos)
{
    for (int i = 0; i < neg_list.size(); i++) {
        if (neg_list[i]->Inside(pos)) {
            double ratio = -1 * neg_list[i]->GetActivity();
            if (Random::Selection(ratio)) {
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

double SourceList::ExpectedDecays(double start_time, double sim_time) const {
    double total = 0;
    for (auto & source: list) {
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

double SourceList::ExpectedPhotons(double start_time, double sim_time) const {
    double total = 0;
    for (auto & source: list) {
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

double SourceList::SearchSplitTime(double start_time, double full_sim_time,
                                   double split_start, double no_photons,
                                   double tol) const
{
    double end_time = start_time + full_sim_time;
    // Initialize to the end of the simulation
    double split_time = end_time - start_time;
    double alpha = 0.5;
    double split_no_photons;
    for (int idx = 0; idx < 20; idx++) {
        split_no_photons = ExpectedPhotons(split_start, split_time);
        split_time *= 1.0 + alpha * ((no_photons / split_no_photons) - 1.0);
        // Numerical safety check
        if ((split_time <= 0) || (split_time > full_sim_time)) {
            break;
        }
        if ((abs(split_no_photons - no_photons) / no_photons) < tol) {
            break;
        }
    }
    // Make sure we don't overrun the end of the simulation, this should not
    // happen, but tolerances can add up.
    if (split_start + split_time > end_time) {
        split_time = end_time - split_start;
    }
    // Always return something sensible
    if (split_time < 0) {
        split_time = 0;
    }
    return(split_time);
}

void SourceList::CalculateEqualPhotonTimeSplits(
    double start_time, double full_sim_time, int n,
    std::vector<double> & split_start, std::vector<double> & split_length) const
{
    split_start = std::vector<double>(n, start_time);
    split_length = std::vector<double>(n, full_sim_time / n);
    double total_photons = ExpectedPhotons(start_time, full_sim_time);
    double split_exp_photons = total_photons / n;
    for (int idx = 0; idx < n; idx++) {
        split_length[idx] = SearchSplitTime(start_time, full_sim_time,
                                            split_start[idx], split_exp_photons,
                                            1.0e-11);
        if ((idx + 1) < n) {
            split_start[idx + 1] = split_start[idx] + split_length[idx];
        }
    }
}

void SourceList::AdjustTimeForSplit(int idx, int n) {
    std::vector<double> split_starts, split_times;
    CalculateEqualPhotonTimeSplits(start_time, simulation_time, n,
                                   split_starts, split_times);
    SetStartTime(split_starts[idx]);
    SetSimulationTime(split_times[idx]);
}

void SourceList::InitSources() {
    for (size_t sidx = 0; sidx < list.size(); sidx++) {
        DecayInfo info;
        info.time = start_time;
        info.source_idx = sidx;
        AddNextDecay(info);
    }
}

void SourceList::BuildMaterialStacks(const SceneDescription & scene) {
    const VectorR3 dir(1, 0, 0);

    // Use the first material listed in Gray_Materials as the default material
    // in the world.
    GammaMaterial const * const default_material(
            static_cast<GammaMaterial const * const>(&scene.GetMaterial(0)));
    for (auto & source: list) {
        stack<GammaMaterial const *> materials;
        stack<bool> front_face;
        VisiblePoint point;
        point.SetPosition(source->GetPosition());

        double hit_dist = DBL_MAX;
        long obj_num = scene.SeekIntersection(
                point.GetPosition() + dir * SceneDescription::ray_trace_epsilon,
                dir, hit_dist, point);

        while (obj_num >= 0) {
            materials.push(static_cast<GammaMaterial const *>(&point.GetMaterial()));
            if (point.IsFrontFacing()) {
                front_face.push(true);
            } else {
                front_face.push(false);
            }
            hit_dist = DBL_MAX;
            obj_num = scene.SeekIntersection(
                    point.GetPosition() + dir * SceneDescription::ray_trace_epsilon,
                    dir, hit_dist, point);
        }

        stack<GammaMaterial const *> true_materials;
        true_materials.push(default_material);
        while (!materials.empty()) {
            bool is_front_face = front_face.top();
            GammaMaterial const * material = materials.top();
            front_face.pop();
            materials.pop();

            if (!is_front_face) {
                true_materials.push(material);
            } else {
                true_materials.pop();
                if (true_materials.size() < 1) {
                    throw runtime_error("Error in determining source materials: potential object overlap error");
                }
            }
        }
        source->SetMaterialStack(true_materials);
    }
}

std::stack<GammaMaterial const *> SourceList::GetSourceMaterialStack(size_t idx) const
{
    return (list[idx]->GetMaterialStack());
}

const GammaMaterial & SourceList::GetSourceMaterial(size_t idx) const {
    return (*list[idx]->GetMaterialStack().top());
}

/*!
 * Checks if a photon's start position changes the geometry it is in relative
 * to the center of the source from which we have already established a
 * reliable materials stack.
 *
 * This calls SeekIntersection limited to the distance between the two points.
 */
std::stack<GammaMaterial const *> SourceList::GetUpdatedStack(
        size_t idx, const VectorR3 & pos, const SceneDescription & scene) const
{
    std::stack<GammaMaterial const *> mat_stack(GetSourceMaterialStack(idx));
    VectorR3 src_pos(list[idx]->GetPosition());
    // If the points are equal, as they will be for a decay without some sort
    // of blur like positron range, then bail without any ray tracing.
    if (src_pos == pos) {
        return (mat_stack);
    }
    // The direction vector from the center of the source to the photon's
    // starting position. Leave this unnormalized for now so we can calculate
    // the distance.
    auto dir = pos - src_pos;
    // This holds the maximum trace distance / returned hit distance
    double dist = dir.Norm();
    // The remaining distance we must trace to the photon's starting point
    double remaining_dist = dist;
    dir.Normalize();
    VisiblePoint point;
    point.SetPosition(src_pos);
    while (scene.SeekIntersection(point.GetPosition() + 1e-10 * dir, dir, dist, point) >= 0)
    {
        remaining_dist -= dist + SceneDescription::ray_trace_epsilon;
        dist = remaining_dist;
        if (point.IsFrontFacing()) {
            // Front face means we are entering a material.
            mat_stack.push(static_cast<GammaMaterial const * const>(&point.GetMaterial()));
        } else if (point.IsBackFacing()) {
            // Back face means we are exiting a material
            if (mat_stack.empty()) {
                // If we somehow have an empty stack, then we somehow missed a
                // front face.
                break;
            }
            if (mat_stack.top() != (&point.GetMaterial())) {
                // If the material we find on the back face isn't the material
                // we think we're in, then there's probably some weird overlap.
                break;
            }
            // If everything looks okay, pull that material off of the stack.
            mat_stack.pop();
        }
    }
    return (mat_stack);
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
        if (gamma_emission_energy < 0) {
            gamma_emission_energy = 0;
        }

        valid_positrons[isotope_name] = Positron(acolinearity, half_life,
                                                 pos_emission_prob,
                                                 gamma_emission_energy);

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
