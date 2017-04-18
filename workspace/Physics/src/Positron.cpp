#include <Physics/Positron.h>

using namespace std;

Positron::Positron(double acolinearity_deg_fwhm, double half_life) :
    Isotope(half_life),
    p(acolinearity_deg_fwhm)
{
}

Positron::Positron(double acolinearity_deg_fwhm, double half_life,
                   double positron_emis_prob) :
    Isotope(half_life),
    p(acolinearity_deg_fwhm, positron_emis_prob)
{
}

Positron::Positron(double acolinearity_deg_fwhm, double half_life,
                   double positron_emis_prob, double gamma_decay_energy_mev) :
    Isotope(half_life),
    p(acolinearity_deg_fwhm, positron_emis_prob, gamma_decay_energy_mev)
{
}

void Positron::Reset() {
    p.Reset();
    Isotope::Reset();
}

void Positron::Decay(int photon_number, double time, int src_id,
                     const VectorR3 & position)
{
    AddNuclearDecay(&p);
    if (use_positron_dbexp) {
        p.Decay(photon_number, time, src_id, position, positronC, positronK1,
                positronK2, positronMaxRange);
    } else if (use_positron_gauss) {
        p.Decay(photon_number, time, src_id, position, positronFWHM,
                positronMaxRange);
    } else {
        p.Decay(photon_number, time, src_id, position);
    }
}
