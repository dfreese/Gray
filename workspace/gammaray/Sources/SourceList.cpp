#include <Sources/SourceList.h>
#include <Random/Random.h>
#include <Physics/Beam.h>
#include <Physics/Positron.h>
#include <Sources/PointSource.h>
#include <Sources/VectorSource.h>
#include <VrMath/LinearR3.h>
#include <algorithm>
#include <exception>
#include <limits>
#include <fstream>
#include <sstream>
#include <json/json.h>

using namespace std;

SourceList::SourceList() :
    decay_number(0),
    simulate_isotope_half_life(true),
    start_time(0)
{
}

void SourceList::AddSource(std::unique_ptr<Source> s)
{
    shared_ptr<Isotope> isotope = valid_isotopes[current_isotope];
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
    valid_isotopes.emplace(iso, new Beam(axis, angle, energy));
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

void SourceList::SetSimulateIsotopeHalfLife(bool val) {
    simulate_isotope_half_life = val;
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

void SourceList::PrintSplits(int n) const {
    std::vector<double> split_starts, split_times;
    CalculateEqualPhotonTimeSplits(
            start_time, simulation_time, n, split_starts, split_times);
    std::cout << "starts:";
    for (auto start : split_starts) {
        std::cout << " " << start;
    }
    std::cout << '\n';
    std::cout << "times:";
    for (auto time: split_times) {
        std::cout << " " << time;
    }
    std::cout << '\n';
}

void SourceList::InitSources() {
    for (int sidx = 0; sidx < static_cast<int>(list.size()); ++sidx) {
        DecayInfo info;
        info.time = start_time;
        info.source_idx = sidx;
        decay_list.emplace(NextDecay(info));
    }
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
    if ((half_life <= 0) && (!simulate_isotope_half_life)) {
        half_life = std::numeric_limits<double>::infinity();
    }

    if ((positron_prob < 0) || (positron_prob > 1.0)) {
        positron_prob = 1.0;
    }
    if (gamma_energy < 0) {
        gamma_energy = 0;
    }

    valid_isotopes[iso_name] = std::unique_ptr<Isotope>(
            new Positron(acolinearity, half_life, positron_prob, gamma_energy));
    Positron cur_positron = dynamic_cast<Positron&>(
            *valid_isotopes[iso_name].get());

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

    return (true);
}

bool SourceList::LoadIsotopes(const std::string& physics_filename) {
    Json::Value root;
    Json::Reader reader;
    std::ifstream input(physics_filename);
    if (!input) {
        cerr << physics_filename << " not found." << endl;
        return(false);
    }
    std::stringstream buffer;
    buffer << input.rdbuf();

    bool status = reader.parse(buffer.str(), root,
                               /*collect_comments=*/false);
    if (!status) {
        std::cerr << "Reading of Physics File, \""
                  << physics_filename << "\" failed\n"
                  << reader.getFormattedErrorMessages() << "\n";
        return (false);
    }

    Json::Value isotopes = root["isotopes"];
    if (isotopes.isNull()) {
        std::cerr << "Reading of Physics File, \""
                  << physics_filename << "\" failed\n"
                  << "Json file does not have \"isotopes\" member\n";
        return (false);
    }

    for (const std::string & iso_name : isotopes.getMemberNames ()) {
        bool status = LoadIsotope(iso_name, isotopes[iso_name]);
        if (!status) {
            std::cerr << "Unable to load isotope, \"" << iso_name << "\"\n";
            return (false);
        }
    }
    return(true);
}
