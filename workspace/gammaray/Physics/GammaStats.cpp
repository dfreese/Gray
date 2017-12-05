#include <Physics/GammaStats.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <Math/Math.h>
#include <Physics/Physics.h>
#include <Random/Random.h>

using namespace std;

GammaStats::GammaStats() :
    cache_energy_min(-1),
    cache_energy_max(-1),
    cache_idx(0),
    num_escape(0),
    material(-1),
    enable_interactions(true),
    log_material(false)
{
}

void GammaStats::SetName(const std::string & n)
{
    name = n;
}

void GammaStats::SetMaterialType(int s)
{
    material=s;
}

void GammaStats::SetFileName(const std::string & n)
{
    filename = n;
    name = n;
}

int GammaStats::GetMaterial() const
{
    return material;
}

std::string GammaStats::GetName() const
{
    return name;
}

bool GammaStats::InteractionsEnabled() const {
    return (enable_interactions);
}

void GammaStats::DisableInteractions()
{
    enable_interactions = false;
}

size_t GammaStats::GetIndex(double e) const
{
    if ((cache_energy_min >= e) && (cache_energy_max < e)) {
        return cache_idx;
    }
    // binary search the sorted list of energies for the index.  This
    // intentionally searches in the same way Math::linear_interpolate does.
    cache_idx = (std::upper_bound(energy.begin(), energy.end(), e) -
                 energy.begin());
    if (cache_idx == energy.size()) {
        cache_energy_max = energy.back();
    } else {
        cache_energy_max = energy[cache_idx];
    }
    if (cache_idx == 0) {
        cache_energy_min = energy.front();
    } else {
        cache_energy_min = energy[cache_idx - 1];
    }
    return cache_idx;
}

bool GammaStats::Load()
{
    ifstream infile(filename);
    if (!infile) {
        cerr << " Unable to open file: " << filename << endl;
        return false;
    }
    string line;
    getline(infile, line);
    stringstream line_stream(line);
    int no_points;
    if ((line_stream >> no_points).fail()) {
        return(false);
    }
    if (0 >= no_points) {
        return(false);
    }

    energy.clear();
    energy.resize(no_points, -1);
    photoelectric.clear();
    photoelectric.resize(no_points, -1);
    compton.clear();
    compton.resize(no_points, -1);
    rayleigh.clear();
    rayleigh.resize(no_points, -1);

    for (int i = 0; i < no_points; i++) {
        string pt_line;
        getline(infile, pt_line);
        stringstream pt_stream(pt_line);
        bool line_fail = false;
        line_fail |= (pt_stream >> energy[i]).fail();
        line_fail |= (pt_stream >> photoelectric[i]).fail();
        line_fail |= (pt_stream >> compton[i]).fail();
        line_fail |= (pt_stream >> rayleigh[i]).fail();

        if (line_fail) {
            cerr << "Error reading file: " << filename << " on line: "
                 << (i + 1) << endl;
            return(false);
        }
    }

    log_energy.resize(no_points);
    log_photoelectric.resize(no_points);
    log_compton.resize(no_points);
    log_rayleigh.resize(no_points);

    auto log_func = [](double & val) { return (std::log(val)); };
    std::transform(energy.begin(), energy.end(),
                   log_energy.begin(), log_func);
    std::transform(photoelectric.begin(), photoelectric.end(),
                   log_photoelectric.begin(), log_func);
    std::transform(compton.begin(), compton.end(),
                   log_compton.begin(), log_func);
    std::transform(rayleigh.begin(), rayleigh.end(),
                   log_rayleigh.begin(), log_func);



    getline(infile, line);
    if (!line.empty()) {
        int xray_lines;
        stringstream xray_lines_ss(line);
        if ((xray_lines_ss >> xray_lines).fail()) {
            cerr << "Error reading the number of xray escape lines" << endl;
            return(false);
        }

        xray_binding_energy.resize(xray_lines);
        xray_emission_energy.resize(xray_lines);
        xray_emission_prob.resize(xray_lines);

        for (int i = 0; i < xray_lines; i++) {
            string pt_line;
            getline(infile, pt_line);
            stringstream pt_stream(pt_line);
            bool line_fail = false;
            line_fail |= (pt_stream >> xray_binding_energy[i]).fail();
            line_fail |= (pt_stream >> xray_emission_energy[i]).fail();
            line_fail |= (pt_stream >> xray_emission_prob[i]).fail();

            if (line_fail) {
                cerr << "Error reading file: " << filename << " on xray line: "
                     << (i + 1) << endl;
                return(false);
            }
        }
    } else {
        // If the Xray Escape Energies are not specified in the file, add in
        // defaults that can be used.  This is equivalent to every file having
        //
        // 1
        //   0.0   0.0   1.0
        //
        // At the end of every file
        xray_binding_energy = vector<double>(1, 0.0);
        xray_emission_energy = vector<double>(1, 0.0);
        xray_emission_prob = vector<double>(1, 1.0);
    }

    xray_emission_cumprob = xray_emission_prob;
    double cum_sum = 0;
    for_each(xray_emission_cumprob.begin(), xray_emission_cumprob.end(),
             [&cum_sum](double & val){cum_sum += val; val = cum_sum;});

    // Hack to make sure all of the probabilities end up summing to 1.
    double total = xray_emission_cumprob.back();
    for_each(xray_emission_prob.begin(), xray_emission_prob.end(),
             [&total](double & val){val /= total;});
    for_each(xray_emission_cumprob.begin(), xray_emission_cumprob.end(),
             [&total](double & val){val /= total;});
    if (!is_sorted(xray_binding_energy.begin(), xray_binding_energy.end()))
    {
        cerr << "XRay Escape binding energies were not in acending order"
        << endl;
        return(false);
    }

    double cur_bind_e = xray_binding_energy.front();
    for (size_t ii = 0; ii < xray_binding_energy.size(); ii++) {
        double bind_e = xray_binding_energy[ii];
        if (cur_bind_e != bind_e) {
            xray_binding_enery_scale.push_back(xray_emission_cumprob[ii]);
            unique_xray_binding_energy.push_back(cur_bind_e);
            cur_bind_e = bind_e;
        }
    }
    xray_binding_enery_scale.push_back(xray_emission_cumprob.back());
    unique_xray_binding_energy.push_back(xray_binding_energy.back());


    return(true);
}

void GammaStats::GetInteractionProbs(double e, double & pe, double & comp,
                                     double & ray) const
{
    size_t idx = GetIndex(e);
    const double log_e = std::log(e);
    pe = std::exp(Math::interpolate(log_energy, log_photoelectric, log_e, idx));
    comp = std::exp(Math::interpolate(log_energy, log_compton, log_e, idx));
    ray = std::exp(Math::interpolate(log_energy, log_rayleigh, log_e, idx));
}

const std::vector<double> & GammaStats::GetXrayEmissionEnergies() const {
    return(xray_emission_energy);
}

const std::vector<double> & GammaStats::GetXrayEmissionCumProb() const {
    return(xray_emission_cumprob);
}

double GammaStats::GetXrayBindEnergyScale(double energy) const {
    auto it = upper_bound(unique_xray_binding_energy.begin(),
                          unique_xray_binding_energy.end(), energy);
    size_t idx = it - unique_xray_binding_energy.begin() - 1;
    return(xray_binding_enery_scale[idx]);
}

double GammaStats::GetComptonScatterAngle(double energy) const {
    return (klein_nishina.scatter_angle(energy, Random::Uniform()));
}

GammaStats::KleinNishina::KleinNishina() :
    // These energies were chosen, as they give less than 0.5% error from 0 to
    // 1.5MeV when linear interpolation is performed.
    energy_idx({
        0.0, 0.010, 0.030, 0.050, 0.100, 0.200, 0.300, 0.400, 0.500, 0.600,
        0.700, 0.900, 1.100, 1.300, 1.500}),
    // Go from -1 to 1 linear in theta
    costheta_idx(Math::cos_space(50)),
    scatter_cdfs(create_scatter_cdfs(energy_idx, costheta_idx))
{
}

/*!
 * Calculates dsigma / dtheta for the Klein-Nishina formula to be used in a
 * accept/reject monte carlo.  For this reason the constants at the front of the
 * formula have been dropped out, as they will be divided out eventually by the
 * max.
 * In this case prob_e_theta is P(E, theta) as seen here:
 * https://en.wikipedia.org/wiki/Klein–Nishina_formula
 * and sigma is dsigma/dtheta = 2 pi * sintheta * dsigma / domega, obtained by
 * integrating over phi.
 *
 */
double GammaStats::KleinNishina::dsigma(const double costheta,
                                     const double energy_mev)
{
    const double alpha = energy_mev / Physics::energy_511;
    const double sintheta2 = 1.0 - costheta * costheta;
    const double sintheta = std::sqrt(sintheta2);
    const double prob_e_theta = 1. / (1. + alpha * (1. - costheta));
    const double sigma = (sintheta * prob_e_theta * prob_e_theta *
                          (prob_e_theta + (1./ prob_e_theta) - sintheta2));
    return(sigma);
}

double GammaStats::KleinNishina::scatter_angle(
        double energy, double rand_uniform) const
{
    return (Math::interpolate_y_2d(energy_idx, costheta_idx, scatter_cdfs,
                                   energy, rand_uniform));
}

std::vector<std::vector<double>> GammaStats::KleinNishina::create_scatter_cdfs(
        const std::vector<double> & energies,
        const std::vector<double> & costhetas)
{
    std::vector<std::vector<double>> scatter_cdfs(
      energies.size(), std::vector<double>(costhetas.size()));

    for (size_t ii = 0; ii < energies.size(); ++ii) {
        const double energy = energies[ii];
        auto & energy_cdf = scatter_cdfs[ii];
        std::transform(costhetas.begin(), costhetas.end(), energy_cdf.begin(),
                       [&energy](double costheta) {
                           return (KleinNishina::dsigma(costheta, energy));
                       });
        // Integrate in theta space, not cos(theta).
        std::vector<double> thetas(costhetas.size());
        std::transform(costhetas.begin(), costhetas.end(), thetas.begin(),
                       [](double theta) { return (std::acos(theta)); });
        energy_cdf = Math::pdf_to_cdf(thetas, energy_cdf);
    }

    return (scatter_cdfs);
}

