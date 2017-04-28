#include <Physics/Positron.h>

using namespace std;

Positron::Positron() :
    Isotope(),
    p(),
    use_positron_dbexp(false),
    use_positron_gauss(false)
{
}


Positron::Positron(double acolinearity_deg_fwhm, double half_life) :
    Isotope(half_life),
    p(acolinearity_deg_fwhm),
    use_positron_dbexp(false),
    use_positron_gauss(false)
{
}

Positron::Positron(double acolinearity_deg_fwhm, double half_life,
                   double positron_emis_prob) :
    Isotope(half_life),
    p(acolinearity_deg_fwhm, positron_emis_prob),
    use_positron_dbexp(false),
    use_positron_gauss(false)
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

void Positron::SetPositronRange(double c, double k1, double k2, double max) {
    use_positron_dbexp = true;
    use_positron_gauss  = false;
    positronC = c;
    positronK1 = k1;
    positronK2 = k2;
    positronMaxRange = max;
}

void Positron::SetPositronRange(double fwhm, double max) {
    use_positron_dbexp = true;
    use_positron_gauss  = false;
    positronFWHM = fwhm;
    positronMaxRange = max;
}

void Positron::set_acolinearity(double acolinearity_deg_fwhm) {
    p.set_acolinearity(acolinearity_deg_fwhm);
}
