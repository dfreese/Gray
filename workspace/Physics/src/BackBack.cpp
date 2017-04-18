#include <Physics/BackBack.h>
#include <limits>

using namespace std;

BackBack::BackBack(double acolinearity_deg_fwhm) :
    GammaPositron(acolinearity_deg_fwhm,
                  std::numeric_limits<double>::infinity()) // Infinite half-life
{
    Reset();
    use_positron_dbexp = false;
    use_positron_gauss = false;
}
