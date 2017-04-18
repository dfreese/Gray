#include <Physics/GammaPositron.h>

using namespace std;

GammaPositron::GammaPositron(double acolinearity_deg_fwhm,
                             double half_life) :
    Isotope(half_life),
    gp(acolinearity_deg_fwhm)
{
}

GammaPositron::GammaPositron(double acolinearity_deg_fwhm,
                             double half_life,
                             double positron_emis_prob) :
    Isotope(half_life),
    gp(acolinearity_deg_fwhm, positron_emis_prob)
{
}

GammaPositron::GammaPositron(double acolinearity_deg_fwhm,
                             double half_life,
                             double positron_emis_prob,
                             double gamma_decay_energy_mev) :
    Isotope(half_life),
    gp(acolinearity_deg_fwhm, positron_emis_prob, gamma_decay_energy_mev)
{
}

void GammaPositron::Reset() {
    gp.Reset();
    Isotope::Reset();
}


void GammaPositron::Decay(int photon_number, double time, int src_id,
                          const VectorR3 & position)
{
    AddNuclearDecay(&gp);
    if (use_positron_dbexp) {
        gp.Decay(photon_number, time, src_id, position, positronC, positronK1,
                 positronK2, positronMaxRange);
    } else if (use_positron_gauss) {
        gp.Decay(photon_number, time, src_id, position, positronFWHM,
                 positronMaxRange);
    } else {
        gp.Decay(photon_number, time, src_id, position);
    }
}
