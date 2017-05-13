#include <Pipeline/blur.h>

const double Blur::sigma_to_fwhm = 2.0 * std::sqrt(2.0 * std::log(2.0));
const double Blur::fwhm_to_sigma = 1.0 / Blur::sigma_to_fwhm;
