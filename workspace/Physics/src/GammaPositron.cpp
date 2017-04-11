#include <Physics/GammaPositron.h>

using namespace std;

GammaPositron::GammaPositron(double acolinearity_deg_fwhm,
                             double half_life,
                             double gamma_decay_energy_mev,
                             double positron_emis_prob) :
    Isotope(half_life),
    gp(acolinearity_deg_fwhm, gamma_decay_energy_mev, positron_emis_prob)
{
}

void GammaPositron::Reset() {
    gp.Reset();
    Isotope::Reset();
}
