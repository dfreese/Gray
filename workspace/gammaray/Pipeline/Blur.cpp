#include <Pipeline/blur.h>

// There seems to be a bug particularly within the blurring functions when Gray
// is run on a 32bit system.  It's not worth trying to trace that down right now
// so until then, don't compile on 32bit systems, which will have an 4byte
// pointer compared to 8 on a 64bit system.
static_assert(sizeof(void*) == 8,
              "Only 64bit compilers are supported at this time");
const double Blur::sigma_to_fwhm = 2.0 * std::sqrt(2.0 * std::log(2.0));
const double Blur::fwhm_to_sigma = 1.0 / Blur::sigma_to_fwhm;
