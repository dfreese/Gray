#include <Physics/KleinNishina.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <Math/Math.h>
#include <Physics/Physics.h>

KleinNishina::KleinNishina() :
    // These energies were chosen, as they give less than 0.5% error from 0 to
    // 1.5MeV when linear interpolation is performed.
    energy_idx({
        0.0, 0.010, 0.030, 0.050, 0.100, 0.200, 0.300, 0.400, 0.500, 0.600,
        0.700, 0.900, 1.100, 1.300, 1.500}),
    // Go from -1 to 1 linear in theta
    costheta_idx(Math::cos_space(300)),
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
double KleinNishina::dsigma(const double costheta, const double energy_mev) {
    const double alpha = energy_mev / Physics::energy_511;
    const double sintheta2 = 1.0 - costheta * costheta;
    const double sintheta = std::sqrt(sintheta2);
    const double prob_e_theta = 1. / (1. + alpha * (1. - costheta));
    const double sigma = (sintheta * prob_e_theta * prob_e_theta *
                          (prob_e_theta + (1./ prob_e_theta) - sintheta2));
    return(sigma);
}

std::vector<double> KleinNishina::dsigma(
        const std::vector<double>& costhetas, const double energy_mev)
{
    std::vector<double> dsigma_dtheta(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), dsigma_dtheta.begin(),
                   [&energy_mev](double costheta) {
                       return (dsigma(costheta, energy_mev));
                   });
    return (dsigma_dtheta);
}

double KleinNishina::scatter_angle(
        double energy, double rand_uniform) const
{
    return (Math::interpolate_y_2d(energy_idx, costheta_idx, scatter_cdfs,
                                   energy, rand_uniform));
}

std::vector<std::vector<double>> KleinNishina::create_scatter_cdfs(
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

