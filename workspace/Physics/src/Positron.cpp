#include <Physics/Positron.h>

using namespace std;

Positron::Positron(double acolinearity_deg_fwhm) :
    p(acolinearity_deg_fwhm)
{
}

void Positron::Reset() {
    p.Reset();
    Isotope::Reset();
}
