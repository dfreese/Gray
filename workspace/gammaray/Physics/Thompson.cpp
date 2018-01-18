#include <Physics/Thompson.h>
#include <algorithm>
#include <cmath>
#include <vector>

/*!
 * Calculates dsigma / dtheta for the Thompson formula.  The constants at the
 * front of the formula have been dropped out, as they will be divided out
 * eventually by the cdf function.
 *
 * sigma is dsigma/dtheta = 2 pi * sintheta * dsigma / domega, obtained by
 * integrating over phi.
 *
 */
double Thompson::dsigma(const double costheta) {
    const double sintheta = std::sqrt(1.0 - costheta * costheta);
    return (sintheta * (1.0 + costheta * costheta));
}

std::vector<double> Thompson::dsigma(const std::vector<double>& costhetas) {
    std::vector<double> dsigma_dtheta(costhetas.size());
    std::transform(costhetas.begin(), costhetas.end(), dsigma_dtheta.begin(),
                   [](double costheta) {
                       return (dsigma(costheta));
                   });
    return (dsigma_dtheta);
}


