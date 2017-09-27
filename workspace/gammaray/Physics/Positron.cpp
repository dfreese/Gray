#include <Physics/Positron.h>
#include <Random/Random.h>

using namespace std;

namespace {
    const double CONST_FWHM_TO_SIGMA = (1.0)/(2.35482005);
    const double CONST_MM_TO_CM = (0.1); // 10 mm per cm
}

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
        VectorR3 anni_position = PositronRangeLevin(position, positronC,
                                                    positronK1, positronK2,
                                                    positronMaxRange);
        p.Decay(photon_number, time, src_id, position, anni_position);
    } else if (use_positron_gauss) {
        VectorR3 anni_position = PositronRangeGauss(position, positronFWHM,
                                                    positronMaxRange);
        p.Decay(photon_number, time, src_id, position, anni_position);
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

VectorR3 Positron::PositronRangeLevin(const VectorR3 & p, double positronC,
                                      double positronK1, double positronK2,
                                      double positronMaxRange)
{
    // First generate a direction that the photon will be blurred
    VectorR3 positronDir = Random::UniformSphere();
    double range;
    // generate cprime which is the scales the dual exponential into a form
    // that allows it to be monte-carlo generated
    double cp = (positronC)/(positronC+positronK1/positronK2*(1-positronC));
    do {
        if (Random::Uniform() < cp) {
            range = Random::Exponential(positronK1);
        } else {
            range = Random::Exponential(positronK2);
        }

    } while (range > positronMaxRange); // rejection test positron range
    range *= CONST_MM_TO_CM;

    positronDir *= range;
    return(p + positronDir);
}

VectorR3 Positron::PositronRangeGauss(const VectorR3 & p, double positronFWHM,
                                      double positronMaxRange)
{
    // First generate a direction that the photon will be blurred
    VectorR3 positronDir = Random::UniformSphere();
    double range = 0.0;
    // must return cm, sigma expressed in mm
    do {
        range = Random::Gaussian() * positronFWHM * CONST_FWHM_TO_SIGMA;
    } while (range > positronMaxRange); // rejection test positron range
    range *= CONST_MM_TO_CM;
    positronDir *= range;

    return(p + positronDir);
}

double Positron::_ExpectedNoPhotons() const {
    double expected = 2.0 * p.PositronEmissionProb();
    if (p.EmitsGamma()) {
        expected += 1.0;
    }
    return(expected);
}
