#include <Physics/BackBack.h>
#include <limits>

using namespace std;

BackBack::BackBack(double acolinearity_deg_fwhm) :
    Positron(acolinearity_deg_fwhm,
                  std::numeric_limits<double>::infinity()) // Infinite half-life
{
    Reset();
}
