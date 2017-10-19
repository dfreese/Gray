#include <Physics/Positron.h>
#include <Physics/Physics.h>
#include <Random/Random.h>
#include <Random/Transform.h>

using namespace std;

namespace {
    const double CONST_FWHM_TO_SIGMA = (1.0)/(2.35482005);
    const double CONST_MM_TO_CM = (0.1); // 10 mm per cm
}

Positron::Positron() :
    Isotope(),
    use_positron_dbexp(false),
    use_positron_gauss(false),
    acolinearity(0),
    gamma_decay_energy(0),
    positron_emission_prob(1.0),
    emit_gamma(false)
{
}


Positron::Positron(double acolinearity_deg_fwhm, double half_life) :
    Positron(acolinearity_deg_fwhm, half_life, 1.0, 0)
{
}

Positron::Positron(double acolinearity_deg_fwhm, double half_life,
                   double positron_emis_prob) :
    Positron(acolinearity_deg_fwhm, half_life, positron_emis_prob, 0)
{
}

Positron::Positron(double acolinearity_deg_fwhm, double half_life,
                   double positron_emis_prob, double gamma_decay_energy_mev) :
    Isotope(half_life),
    use_positron_dbexp(false),
    use_positron_gauss(false),
    acolinearity(acolinearity_deg_fwhm / 180.0 * M_PI * Transform::fwhm_to_sigma),
    gamma_decay_energy(gamma_decay_energy_mev),
    positron_emission_prob(positron_emis_prob),
    emit_gamma(gamma_decay_energy_mev > 0)
{
}

void Positron::Decay(int photon_number, double time, int src_id,
                     const VectorR3 & position)
{
    VectorR3 anni_position;
    if (use_positron_dbexp) {
        anni_position = PositronRangeLevin(position, positronC,
                                           positronK1, positronK2,
                                           positronMaxRange);
    } else if (use_positron_gauss) {
        anni_position = PositronRangeGauss(position, positronFWHM,
                                           positronMaxRange);
    } else {
        anni_position = position;
    }
    // TODO: log the positron annihilation and nuclear decay positions
    // separately
    NuclearDecay p(photon_number, time, src_id, anni_position, 0.120);

    if (emit_gamma) {
        // TODO: correctly set the time on the gamma decay, based on the
        // lifetime of the intermediate decay state.
        p.AddPhoton(Photon(position, Random::UniformSphere(),
                           gamma_decay_energy, time, photon_number,
                           Photon::P_YELLOW, src_id));
    }

    // Check to see if a Positron was emitted with the gamma or not.
    if (Random::Selection(positron_emission_prob)) {
        const VectorR3 dir = Random::UniformSphere();
        p.AddPhoton(Photon(anni_position, dir,
                           Physics::energy_511, time, photon_number,
                           Photon::P_BLUE, src_id));
        p.AddPhoton(Photon(anni_position,
                           Random::Acolinearity(dir, acolinearity),
                           Physics::energy_511, time, photon_number,
                           Photon::P_RED, src_id));
    }

    this->AddNuclearDecay(std::move(p));
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
        if (Random::Selection(cp)) {
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
    double expected = 2.0 * positron_emission_prob;
    if (emit_gamma) {
        expected += 1.0;
    }
    return(expected);
}
