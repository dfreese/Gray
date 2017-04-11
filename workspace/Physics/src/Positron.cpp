#include <Physics/Positron.h>

using namespace std;

Positron::Positron(double acolinearity_deg_fwhm,
                   double half_life,
                   double positron_emis_prob) :
    Isotope(half_life),
    p(acolinearity_deg_fwhm, positron_emis_prob)
{
}

void Positron::Reset() {
    p.Reset();
    Isotope::Reset();
}
