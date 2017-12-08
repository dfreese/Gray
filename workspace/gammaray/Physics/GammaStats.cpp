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
    index(-1),
    enable_interactions(true),
    log_material(false),
    // Make the form factor always 1
    compton_scatter({0.0, 1.0}, {1.0, 1.0}),
    rayleigh_scatter({0.0, 1.0}, {1.0, 1.0}),
    cache_energy_min(-1),
    cache_energy_max(-1),
    cache_idx(0)
{
}

GammaStats::GammaStats(
    const std::string & name, int index,
    double density, bool sensitive, std::vector<double> energy,
    std::vector<double> matten_comp, std::vector<double> matten_phot,
    std::vector<double> matten_rayl, std::vector<double> x,
    std::vector<double> form_factor, std::vector<double> scattering_func) :
        name(name),
        index(index),
        energy(energy),
        photoelectric(matten_phot),
        compton(matten_comp),
        rayleigh(matten_rayl),
        log_energy(energy.size()),
        log_photoelectric(matten_phot.size()),
        log_compton(matten_comp.size()),
        log_rayleigh(matten_rayl.size()),
        x(x),
        form_factor(form_factor),
        scattering_func(scattering_func),
        enable_interactions(true),
        log_material(sensitive),
        compton_scatter(x, scattering_func),
        rayleigh_scatter(x, form_factor),
        cache_energy_min(-1),
        cache_energy_max(-1),
        cache_idx(0)
{
    // Convert the mass attenuation coefficient to a linear attenuation
    // coefficient by multiplying by density.
    std::transform(photoelectric.begin(), photoelectric.end(),
                   photoelectric.begin(),
                   std::bind1st(std::multiplies<double>(),density));
    std::transform(compton.begin(), compton.end(),
                   compton.begin(),
                   std::bind1st(std::multiplies<double>(),density));
    std::transform(rayleigh.begin(), rayleigh.end(),
                   rayleigh.begin(),
                   std::bind1st(std::multiplies<double>(),density));

    // Cache the log values for log interpolation
    auto log_func = [](double & val) { return (std::log(val)); };
    std::transform(energy.begin(), energy.end(),
                   log_energy.begin(), log_func);
    std::transform(photoelectric.begin(), photoelectric.end(),
                   log_photoelectric.begin(), log_func);
    std::transform(compton.begin(), compton.end(),
                   log_compton.begin(), log_func);
    std::transform(rayleigh.begin(), rayleigh.end(),
                   log_rayleigh.begin(), log_func);
}

void GammaStats::SetName(const std::string & n)
{
    name = n;
}

void GammaStats::SetMaterialType(int s)
{
    index = s;
}

void GammaStats::SetFileName(const std::string & n)
{
    filename = n;
    name = n;
}

int GammaStats::GetMaterial() const
{
    return index;
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

double GammaStats::GetComptonScatterAngle(double energy) const {
    return (compton_scatter.scatter_angle(energy, Random::Uniform()));
}

double GammaStats::GetRayleighScatterAngle(double energy) const {
    return (rayleigh_scatter.scatter_angle(energy, Random::Uniform()));
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

std::vector<double> GammaStats::KleinNishina::dsigma(
        const std::vector<double>& costhetas, const double energy_mev)
{
    std::vector<double> dsigma_dtheta(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), dsigma_dtheta.begin(),
                   [&energy_mev](double costheta) {
                       return (dsigma(costheta, energy_mev));
                   });
    return (dsigma_dtheta);
}

double GammaStats::KleinNishina::scatter_angle(
        double energy, double rand_uniform) const
{
    return (Math::interpolate_y_2d(energy_idx, costheta_idx, scatter_cdfs,
                                   energy, rand_uniform));
}

std::vector<std::vector<double>> GammaStats::KleinNishina::create_scatter_cdfs(
        const std::vector<double>& energies,
        const std::vector<double>& costhetas)
{
    std::vector<std::vector<double>> scatter_cdfs(energies.size());

    // Integrate in theta space, not cos(theta).
    std::vector<double> thetas(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), thetas.begin(),
                   [](double theta) { return (std::acos(theta)); });

    for (size_t ii = 0; ii < energies.size(); ++ii) {
        const double energy = energies[ii];
        auto & energy_cdf = scatter_cdfs[ii];
        energy_cdf = dsigma(costhetas, energy);
        energy_cdf = Math::pdf_to_cdf(thetas, energy_cdf);
    }
    return (scatter_cdfs);
}


GammaStats::Compton::Compton(const std::vector<double>& x,
                             const std::vector<double>& form_factor) :
    // These energies were chosen, as they give less than 0.5% error from 0 to
    // 1.5MeV when linear interpolation is performed.
    energy_idx({
        0.0, 0.010, 0.030, 0.050, 0.100, 0.200, 0.300, 0.400, 0.500, 0.600,
        0.700, 0.900, 1.100, 1.300, 1.500}),
    // Go from -1 to 1 linear in theta
    costheta_idx(Math::cos_space(50)),
    scatter_cdfs(create_scatter_cdfs(energy_idx, costheta_idx, x, form_factor))
{
}

double GammaStats::Compton::scatter_angle(
        double energy, double rand_uniform) const
{
    return (Math::interpolate_y_2d(energy_idx, costheta_idx, scatter_cdfs,
                                   energy, rand_uniform));
}

double GammaStats::Compton::x_val(double cos_theta, double energy_mev) {
    // Planck's contant times the speed of light in MeV*cm
    constexpr double hc_MeV_cm = 1.23984193e-10;
    return (std::sqrt(0.5 * (1.0 - cos_theta)) * energy_mev / hc_MeV_cm);
}

std::vector<double> GammaStats::Compton::x_val(
        const std::vector<double>& costhetas,
        const double energy_mev)
{
    std::vector<double> dsigma_dtheta(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), dsigma_dtheta.begin(),
                   [&energy_mev](double costheta) {
                       return (x_val(costheta, energy_mev));
                   });
    return (dsigma_dtheta);
}

std::vector<double> GammaStats::Compton::scattering(
        const std::vector<double>& costhetas,
        const double energy_mev,
        const std::vector<double>& x,
        const std::vector<double>& scattering_func)
{
    std::vector<double> scattering_vals = x_val(costhetas, energy_mev);
    std::transform(scattering_vals.begin(), scattering_vals.end(),
                   scattering_vals.begin(),
                   [&x, &scattering_func](double x_val) {
                       return (Math::interpolate(x, scattering_func, x_val));
                   });
    return (scattering_vals);
}

std::vector<double> GammaStats::Compton::dsigma(
        const std::vector<double>& costhetas,
        const double energy_mev,
        const std::vector<double>& x,
        const std::vector<double>& scattering_func)
{
    std::vector<double> dsigma_dtheta(
            KleinNishina::dsigma(costhetas, energy_mev));
    std::vector<double> scattering_vals(
            scattering(costhetas, energy_mev, x, scattering_func));
    // Multiply all of the dsigma_dtheta vals by the respective scattering func
    // value.
    auto sf_val = scattering_vals.begin();
    for (double & ds_val : dsigma_dtheta) {
        ds_val *= *(sf_val++);
    }
    return (dsigma_dtheta);
}

std::vector<std::vector<double>> GammaStats::Compton::create_scatter_cdfs(
        const std::vector<double>& energies,
        const std::vector<double>& costhetas,
        const std::vector<double>& x,
        const std::vector<double>& scattering_func)
{
    // Integrate in theta space, not cos(theta).
    std::vector<double> thetas(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), thetas.begin(),
                   [](double theta) { return (std::acos(theta)); });

    std::vector<std::vector<double>> scatter_cdfs(energies.size());
    for (size_t ii = 0; ii < energies.size(); ++ii) {
        const double energy = energies[ii];
        auto & energy_cdf = scatter_cdfs[ii];
        energy_cdf = dsigma(costhetas, energy, x, scattering_func);
        energy_cdf = Math::pdf_to_cdf(thetas, energy_cdf);
    }

    return (scatter_cdfs);
}


/*!
 * Calculates dsigma / dtheta for the Thompson formula.  The constants at the
 * front of the formula have been dropped out, as they will be divided out
 * eventually by the cdf function.
 *
 * sigma is dsigma/dtheta = 2 pi * sintheta * dsigma / domega, obtained by
 * integrating over phi.
 *
 */
double GammaStats::Thompson::dsigma(const double costheta)
{
    const double sintheta = std::sqrt(1.0 - costheta * costheta);
    return (sintheta * (1.0 + costheta * costheta));
}

std::vector<double> GammaStats::Thompson::dsigma(
        const std::vector<double>& costhetas)
{
    std::vector<double> dsigma_dtheta(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), dsigma_dtheta.begin(),
                   [](double costheta) {
                       return (dsigma(costheta));
                   });
    return (dsigma_dtheta);
}

GammaStats::Rayleigh::Rayleigh(
        const std::vector<double>& x,
        const std::vector<double>& form_factor) :
    energy_idx({
        0.0, 0.001, 0.002, 0.005, 0.010, 0.020, 0.040, 0.060, 0.080, 0.080,
        0.100, 0.200, 0.300, 0.500, 1.000}),
    // Go from -1 to 1 linear in theta
    costheta_idx(Math::cos_space(50)),
    scatter_cdfs(create_scatter_cdfs(energy_idx, costheta_idx, x, form_factor))
{
}

double GammaStats::Rayleigh::scatter_angle(
        double energy, double rand_uniform) const
{
    return (Math::interpolate_y_2d(energy_idx, costheta_idx, scatter_cdfs,
                                   energy, rand_uniform));
}

std::vector<double> GammaStats::Rayleigh::formfactor(
        const std::vector<double>& costhetas,
        const double energy_mev,
        const std::vector<double>& x,
        const std::vector<double>& form_factor)
{
    std::vector<double> formfactor_vals(Compton::x_val(costhetas, energy_mev));
    std::transform(formfactor_vals.begin(), formfactor_vals.end(),
                   formfactor_vals.begin(),
                   [&x, &form_factor](double x_val) {
                       return (Math::interpolate(x, form_factor, x_val));
                   });
    return (formfactor_vals);
}

std::vector<double> GammaStats::Rayleigh::dsigma(
        const std::vector<double>& costhetas,
        const double energy_mev,
        const std::vector<double>& x,
        const std::vector<double>& form_factor)
{
    std::vector<double> dsigma_dtheta(Thompson::dsigma(costhetas));
    std::vector<double> formfactor_vals(
        formfactor(costhetas, energy_mev, x, form_factor));
    // Multiply all of the dsigma_dtheta vals by the respective scattering func
    // value.
    auto ff_iter = formfactor_vals.begin();
    for (double & ds_val : dsigma_dtheta) {
        double ff_val = *(ff_iter++);
        ds_val *= ff_val * ff_val;
    }
    return (dsigma_dtheta);
}

std::vector<std::vector<double>> GammaStats::Rayleigh::create_scatter_cdfs(
        const std::vector<double>& energies,
        const std::vector<double>& costhetas,
        const std::vector<double>& x,
        const std::vector<double>& form_factor)
{
    // Integrate in theta space, not cos(theta).
    std::vector<double> thetas(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), thetas.begin(),
                   [](double theta) { return (std::acos(theta)); });

    std::vector<std::vector<double>> scatter_cdfs(energies.size());
    for (size_t ii = 0; ii < energies.size(); ++ii) {
        const double energy = energies[ii];
        auto & energy_cdf = scatter_cdfs[ii];
        energy_cdf = dsigma(costhetas, energy, x, form_factor);
        energy_cdf = Math::pdf_to_cdf(thetas, energy_cdf);
    }

    return (scatter_cdfs);
}
