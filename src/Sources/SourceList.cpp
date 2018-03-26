/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Sources/SourceList.h"
#include "Gray/Random/Random.h"
#include "Gray/Physics/Beam.h"
#include "Gray/Physics/Positron.h"
#include "Gray/Sources/PointSource.h"
#include "Gray/Sources/VectorSource.h"
#include "Gray/VrMath/LinearR3.h"
#include <algorithm>
#include <exception>
#include <limits>
#include <fstream>
#include <sstream>
#include "Gray/json/json.h"

using namespace std;

void SourceList::AddSource(std::unique_ptr<Source> s) {
    shared_ptr<const Isotope> isotope = valid_isotopes[current_isotope];
    if (isotope == nullptr) {
        string error = "Isotope named " + current_isotope
            + " somehow set as current isotope, but was not implemented";
        throw(runtime_error(error));
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

void SourceList::AddIsotope(
        const std::string& name,
        std::unique_ptr<Isotope> s)
{
    valid_isotopes.emplace(name, std::move(s));
}

size_t SourceList::NumSources() const {
    return (list.size());
}

std::shared_ptr<const Source> SourceList::GetSource(size_t idx) const {
    return (list[idx]);
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
    return(decay_list.top().time);
}

double SourceList::GetElapsedTime() const {
    return(GetTime() - start_time);
}

double SourceList::GetSimulationTime() const
{
    return(simulation_time);
}

double SourceList::GetEndTime() const {
    return(end_time);
}

bool SourceList::SimulationIncomplete() const {
    return (GetTime() < GetEndTime());
}

std::vector<VectorR3> SourceList::GetSourcePositions() const {
    std::vector<VectorR3> positions(list.size());
    std::transform(list.begin(), list.end(), positions.begin(),
                   [](const std::shared_ptr<const Source> & s) {
                       return s->GetPosition();
                   });
    return (positions);
}

SourceList::DecayInfo SourceList::NextDecay(DecayInfo base_info) const {
    // Calculating the next source decay timesize_t source_idx, double base_time
    auto & source = list[base_info.source_idx];
    do {
        double source_activity_bq = source->GetActivity(base_info.time);
        // Time advances even if the decay is rejected by the inside negative
        // source test.  This is by design, as we do not know how much activity
        // a negative source inherently removes from the positive sources.
        base_info.time += Random::Exponential(source_activity_bq);
        base_info.position = source->Decay();
    } while (InsideNegative(base_info.position));
    return (base_info);
}

SourceList::DecayInfo SourceList::GetNextDecay() {
    DecayInfo ret_val(decay_list.top());
    decay_list.pop();
    decay_list.emplace(NextDecay(ret_val));
    return (ret_val);
}

NuclearDecay SourceList::Decay() {
    if (list.empty()) {
        string error = "Decay called with no sources to decay";
        throw(runtime_error(error));
    }

    DecayInfo decay = GetNextDecay();

    const Isotope& isotope = list[decay.source_idx]->GetIsotope();
    return (isotope.Decay(
            decay_number++, decay.time, decay.source_idx, decay.position));
}

bool SourceList::InsideNegative(const VectorR3 & pos) const {
    for (const auto& source : neg_list) {
        if (source->Inside(pos)) {
            double ratio = -1 * source->GetActivity();
            if (Random::Selection(ratio)) {
                return true;
            }
        }
    }
    return false;
}

bool SourceList::CreateBeamIsotope(const std::string& iso,
                                   const RigidMapR3& current_matrix) {
    stringstream ss(iso);
    std::string beam_str;
    VectorR3 axis;
    double angle;
    double energy;

    ss >> beam_str;
    ss >> axis.x;
    ss >> axis.y;
    ss >> axis.z;
    ss >> angle;
    ss >> energy;
    if (beam_str != "beam") {
        return (false);
    }
    if (ss.fail()) {
        std::cerr << "Invalid beam. \"" << iso << "\"\n"
                  << "format: \"beam [axis] [angle] [energy]\"\n";
        return (false);
    }
    current_matrix.Transform3x3(&axis);
    AddIsotope(iso, std::unique_ptr<Isotope>(new Beam(axis, angle, energy)));
    return (true);
}

bool SourceList::SetCurIsotope(const std::string& iso,
                               const RigidMapR3& current_matrix)
{
    if (valid_isotopes.count(iso)) {
        current_isotope = iso;
        return(true);
    } else if (CreateBeamIsotope(iso, current_matrix)) {
        current_isotope = iso;
        return (true);
    } else {
        return(false);
    }
}

void SourceList::SetSimulationTime(double time)
{
    simulation_time = time;
    end_time = start_time + simulation_time;
}

void SourceList::DisableHalfLife() {
    simulate_isotope_half_life = false;
    for (auto& iso_pair : valid_isotopes) {
        // This very deliberatly breaks the const-ness of the isotopes for the
        // sourcelist, as we assume that const-ness was only for threading
        // purposes.  This only occurs during loading of the scene file, and
        // there is only one thread running at that time.  This is a dirty hack
        // but I didn't want to, again, rewrite the loading to process all of
        // the things needed for loading of isotopes/materials.  We actually
        // end up doing the same thing with disabling of Rayleigh scattering, I
        // just didn't think about it because the SceneDescription class isn't
        // as careful about only exposing const members after initialization.
        Isotope& iso = const_cast<Isotope&>(*iso_pair.second);
        iso.DisableHalfLife();
    }
}

void SourceList::SetStartTime(double val)
{
    start_time = val;
    end_time = start_time + simulation_time;
}

double SourceList::ExpectedPhotons(double start_time, double sim_time) const {
    double total = 0;
    for (const auto& source: list) {
        total += source->GetExpectedPhotons(start_time, sim_time);
    }
    return (total);
}

double SourceList::SearchSplitTime(
        double start_time, double full_sim_time,
        double split_start, double no_photons) const
{
    double end_time = start_time + full_sim_time;
    // Initialize to the end of the simulation
    double split_time = end_time - start_time;
    double alpha = 0.5;
    for (int idx = 0; idx < 30; idx++) {
        double split_no_photons = ExpectedPhotons(split_start, split_time);
        split_time *= 1.0 + alpha * ((no_photons / split_no_photons) - 1.0);
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
    split_start = std::vector<double>(n);
    split_length = std::vector<double>(n);
    double total_photons = ExpectedPhotons(start_time, full_sim_time);
    double split_exp_photons = total_photons / n;
    for (int idx = 0; idx < n; ++idx) {
        if (idx == 0) {
            split_start[idx] = start_time;
        } else {
            split_start[idx] = split_start[idx - 1] + split_length[idx - 1];
        }
        split_length[idx] = SearchSplitTime(
                start_time, full_sim_time, split_start[idx], split_exp_photons);
    }
}

void SourceList::AdjustTimeForSplit(int idx, int n) {
    std::vector<double> split_starts, split_times;
    CalculateEqualPhotonTimeSplits(start_time, simulation_time, n,
                                   split_starts, split_times);
    SetStartTime(split_starts[idx]);
    SetSimulationTime(split_times[idx]);
}

bool SourceList::PrintSplits(int n) const {
    if (simulation_time <= 0) {
        std::cerr << "Error: unable to calculate splits for simulation time"
            << " of " << simulation_time << "s\n";
        return (false);
    }
    std::vector<double> split_starts, split_times;
    CalculateEqualPhotonTimeSplits(
            start_time, simulation_time, n, split_starts, split_times);
    const std::string output_name("gray_splits.dat");
    std::ofstream output(output_name);
    if (!output) {
        std::cerr << "Unable to open file: \"" << output_name << "\"\n";
        return (false);
    }
    output << "# start split\n";
    for (int idx = 0; idx < n; ++idx) {
        output << split_starts[idx] << " " << split_times[idx] << "\n";
    }

    std::cout << "splits written to \"" << output_name << "\"\n";
    return (true);
}

void SourceList::InitSources() {
    for (int sidx = 0; sidx < static_cast<int>(list.size()); ++sidx) {
        DecayInfo info;
        info.time = start_time;
        info.source_idx = sidx;
        decay_list.emplace(NextDecay(info));
    }
}

std::unique_ptr<Isotope> SourceList::IsotopeFactory(
        Json::Value isotope, bool simulate_isotope_half_life)
{
    auto required_vals = {
        "acolinearity_deg_fwhm",
        "half_life_s",
        "model",
        "positron_emiss_prob",
        "prompt_gamma_energy_mev"
    };
    for (auto val : required_vals) {
        if (isotope[val].isNull()) {
            std::cerr << "Required key, \"" << val << "\" does not exist.\n";
            return (nullptr);
        }
    }
    double acolinearity = isotope["acolinearity_deg_fwhm"].asDouble();
    double half_life = isotope["half_life_s"].asDouble();
    double positron_prob = isotope["positron_emiss_prob"].asDouble();
    double gamma_energy = isotope["prompt_gamma_energy_mev"].asDouble();
    std::string model = isotope["model"].asString();

    // If the value given is negative or the user disables source decay from
    // the command line, then set the half life to be infinite.
    if ((half_life <= 0.0) || (!simulate_isotope_half_life)) {
        half_life = std::numeric_limits<double>::infinity();
    }

    if ((positron_prob < 0) || (positron_prob > 1.0)) {
        positron_prob = 1.0;
    }
    if (gamma_energy < 0) {
        gamma_energy = 0;
    }

    std::unique_ptr<Isotope> iso = std::unique_ptr<Isotope>(
            new Positron(acolinearity, half_life, positron_prob, gamma_energy));
    Positron& cur_positron = dynamic_cast<Positron&>(*iso.get());

    if (model == "none") {
        // Currently do nothing...
    } else if (model == "gauss") {
        Json::Value fwhm_mm_json = isotope["fwhm_mm"];
        Json::Value max_mm_json = isotope["max_range_mm"];
        if (!fwhm_mm_json.isDouble() || !max_mm_json.isDouble()) {
            std::cerr << "gauss model requires \"fwhm_mm\" and"
                      << "\"max_range_mm\" double values\n";
            return (nullptr);
        }

        double fwhm_mm = fwhm_mm_json.asDouble();
        double max_mm = max_mm_json.asDouble();
        cur_positron.SetPositronRange(fwhm_mm, max_mm);
    } else if (model == "levin_exp") {
        Json::Value c_json = isotope["prob_c"];
        Json::Value k1_json = isotope["k1"];
        Json::Value k2_json = isotope["k2"];
        Json::Value max_mm_json = isotope["max_range_mm"];
        if (!c_json.isDouble() || !k1_json.isDouble() ||
            !k2_json.isDouble() || !max_mm_json.isDouble()) {
            std::cerr << "levin_exp model requires \"prob_c\", "
                         "\"k1\", \"k2\", and \"max_range_mm\" "
                         "double values\n";
            return (nullptr);
        }
        double c = c_json.asDouble();
        double k1 = k1_json.asDouble();
        double k2 = k2_json.asDouble();
        double max_mm = max_mm_json.asDouble();
        cur_positron.SetPositronRange(c, k1, k2, max_mm);
    } else {
        std::cerr << "Unrecognized model, \"" << model << "\"\n";
        return (nullptr);
    }

    return (iso);
}

bool SourceList::LoadIsotope(const std::string & iso_name,
                             Json::Value isotope)
{
    auto required_vals = {
        "acolinearity_deg_fwhm",
        "half_life_s",
        "model",
        "positron_emiss_prob",
        "prompt_gamma_energy_mev"
    };
    for (auto val : required_vals) {
        if (isotope[val].isNull()) {
            std::cerr << "Required key, \"" << val << "\" does not exist in "
                      << iso_name << "\n";
            return (false);
        }
    }
    double acolinearity = isotope["acolinearity_deg_fwhm"].asDouble();
    double half_life = isotope["half_life_s"].asDouble();
    double positron_prob = isotope["positron_emiss_prob"].asDouble();
    double gamma_energy = isotope["prompt_gamma_energy_mev"].asDouble();
    std::string model = isotope["model"].asString();

    // If the value given is negative or the user disables source decay from
    // the command line, then set the half life to be infinite.
    if ((half_life <= 0.0) || (!simulate_isotope_half_life)) {
        half_life = std::numeric_limits<double>::infinity();
    }

    if ((positron_prob < 0) || (positron_prob > 1.0)) {
        positron_prob = 1.0;
    }
    if (gamma_energy < 0) {
        gamma_energy = 0;
    }

    auto iso = std::unique_ptr<Isotope>(
            new Positron(acolinearity, half_life, positron_prob, gamma_energy));
    Positron& cur_positron = dynamic_cast<Positron&>(*iso.get());

    Json::Value is_default = isotope["default"];
    if (is_default.isBool()) {
        current_isotope = iso_name;
    }

    if (model == "none") {
        // Currently do nothing...
    } else if (model == "gauss") {
        Json::Value fwhm_mm_json = isotope["fwhm_mm"];
        Json::Value max_mm_json = isotope["max_range_mm"];
        if (!fwhm_mm_json.isDouble() || !max_mm_json.isDouble()) {
            std::cerr << "gauss model requires \"fwhm_mm\" and"
                      << "\"max_range_mm\" double values\n";
            return (false);
        }

        double fwhm_mm = fwhm_mm_json.asDouble();
        double max_mm = max_mm_json.asDouble();
        cur_positron.SetPositronRange(fwhm_mm, max_mm);
    } else if (model == "levin_exp") {
        Json::Value c_json = isotope["prob_c"];
        Json::Value k1_json = isotope["k1"];
        Json::Value k2_json = isotope["k2"];
        Json::Value max_mm_json = isotope["max_range_mm"];
        if (!c_json.isDouble() || !k1_json.isDouble() ||
            !k2_json.isDouble() || !max_mm_json.isDouble()) {
            std::cerr << "levin_exp model requires \"prob_c\", "
                         "\"k1\", \"k2\", and \"max_range_mm\" "
                         "double values\n";
            return (false);
        }
        double c = c_json.asDouble();
        double k1 = k1_json.asDouble();
        double k2 = k2_json.asDouble();
        double max_mm = max_mm_json.asDouble();
        cur_positron.SetPositronRange(c, k1, k2, max_mm);
    } else {
        std::cerr << "Unrecognized model, \"" << model << "\" for isotope \""
                  << iso_name << "\"\n";
        return (false);
    }

    AddIsotope(iso_name, std::move(iso));
    return (true);
}

bool SourceList::LoadIsotopes(const std::string& physics_filename) {
    std::ifstream input(physics_filename);
    if (!input) {
        cerr << physics_filename << " not found." << endl;
        return(false);
    }
    return (LoadIsotopes(input));
}

bool SourceList::LoadIsotopes(std::istream& input) {
    if (!input) {
        return (false);
    }
    Json::Value root;
    Json::Reader reader;
    bool status = reader.parse(input, root, /*collect_comments=*/false);
    if (!status) {
        std::cerr << "Reading of physics file failed\n"
                  << reader.getFormattedErrorMessages() << "\n";
        return (false);
    }

    Json::Value isotopes = root["isotopes"];
    if (isotopes.isNull()) {
        std::cerr << "Physics Json File does not have \"isotopes\" member\n";
        return (false);
    }

    for (const std::string & iso_name : isotopes.getMemberNames ()) {
        Json::Value isotope = isotopes[iso_name];
        auto iso_ptr = IsotopeFactory(isotope, simulate_isotope_half_life);
        if (!iso_ptr) {
            std::cerr << "Unable to load isotope, \"" << iso_name << "\"\n";
            return (false);
        }
        AddIsotope(iso_name, std::move(iso_ptr));
        Json::Value is_default = isotope["default"];
        if (is_default.isBool() && is_default.asBool()) {
            current_isotope = iso_name;
        }
    }

    return(true);
}
