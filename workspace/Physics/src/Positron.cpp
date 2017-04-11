#include <Physics/Positron.h>

using namespace std;

Positron::Positron(double acolinearity_deg_fwhm, double half_life) :
    Isotope(half_life),
    p(acolinearity_deg_fwhm)
{
}

void Positron::Reset() {
    p.Reset();
    Isotope::Reset();
}
