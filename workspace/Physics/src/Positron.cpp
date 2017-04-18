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


void Positron::Decay(int photon_number, double time, int src_id,
                     const VectorR3 & position)
{
    p.Reset();
    AddNuclearDecay(&p);
    if (use_positron_dbexp) {
        p.Decay(photon_number, time, src_id, position, positronC, positronK1,
                positronK2, positronMaxRange);
    } else if (use_positron_gauss) {
        p.Decay(photon_number, time, src_id, position, use_positron_gauss,
                positronMaxRange);
    } else {
        p.Decay(photon_number, time, src_id, position);
    }
}
