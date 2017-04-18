#include <Physics/PositronDecay.h>
#include <stdio.h>
#include <math.h>
#include <Random/Random.h>

using namespace std;

namespace {
    const double CONST_FWHM_TO_SIGMA = (1.0)/(2.35482005);
    const double CONST_MM_TO_CM = (0.1); // 10 mm per cm
}

const double PositronDecay::default_acolinearity = 0.47;

PositronDecay::PositronDecay(double acolinearity_deg_fwhm) :
    acolinearity(acolinearity_deg_fwhm / 180.0 * M_PI * CONST_FWHM_TO_SIGMA),
    gamma_decay_energy(-1.0),
    positron_emission_prob(1.0),
    gamma_position_set(false),
    emit_gamma(false)
{
}

PositronDecay::PositronDecay(double acolinearity_deg_fwhm,
                                       double positron_emis_prob) :
    acolinearity(acolinearity_deg_fwhm / 180.0 * M_PI * CONST_FWHM_TO_SIGMA),
    gamma_decay_energy(-1.0),
    positron_emission_prob(positron_emis_prob),
    gamma_position_set(false),
    emit_gamma(false)
{
}

PositronDecay::PositronDecay(double acolinearity_deg_fwhm,
                                       double positron_emis_prob,
                                       double gamma_decay_energy_mev) :
    acolinearity(acolinearity_deg_fwhm / 180.0 * M_PI * CONST_FWHM_TO_SIGMA),
    gamma_decay_energy(gamma_decay_energy_mev),
    positron_emission_prob(positron_emis_prob),
    gamma_position_set(false),
    emit_gamma(true)
{
}

void PositronDecay::Decay(int photon_number, double time, int src_id,
                               const VectorR3 & position)
{
    // 120keV positron energy for FDG
    // TODO: put an energy in the Positron class that is overridden by it's
    // children.  Pass it during the call to PositronDecay::Decay, and
    // generate an energy for the PositronDecay instance randomly from a
    // beta decay spectrum.
    this->energy = 0.120;
    this->decay_number = photon_number;
    this->src_id = src_id;
    this->time = time;

    if (emit_gamma) {
        // If the gamma position was already set by the positron range
        // functions, do not override it.  Once we've set the position, clear
        // the flag for the next decay call.
        // TODO: log the positron annihilation and nuclear decay positions
        // separately
        if (!gamma_position_set) {
            yellow.pos = position;
            this->position = position;
        } else {
            gamma_position_set = false;
        }
        // TODO: correctly set the time on the gamma decay, based on the
        // lifetime of the intermediate decay state.
        yellow.time = time;
        yellow.energy = gamma_decay_energy;
        yellow.id = photon_number;
        yellow.det_id = -1;
        yellow.src_id = src_id;
        yellow.phantom_scatter = false;
        yellow.color = Photon::P_YELLOW;
        AddPhoton(&yellow);
    }

    // Check to see if a Positron was emitted with the gamma or not.
    if (Random::Uniform() < positron_emission_prob) {
        blue.time = time;
        blue.pos = position;
        blue.energy = ENERGY_511;
        blue.id = photon_number;
        blue.det_id = -1;
        blue.src_id = src_id;
        blue.phantom_scatter = false;
        red = blue;

        blue.SetBlue();
        red.SetRed();

        Random::UniformSphere(blue.dir);
        Random::Acolinearity(blue.dir, red.dir, acolinearity);
        AddPhoton(&blue);
        AddPhoton(&red);
    }
}

void PositronDecay::Decay(int photon_number, double time, int src_id,
                          VectorR3 position, double positronC,
                          double positronK1, double positronK2,
                          double positronMaxRange)
{
    // We set the gamma position here, prior to the positron range funtion,
    // because we assume the gamma is emitted from the nucleus, rather than the
    // positron.
    yellow.pos = position;
    this->position = position;
    // And set a flag to make sure the position is not overridden by the next
    // Decay call.
    gamma_position_set = true;
    PositronDecay::PositronRange(position, positronC, positronK1, positronK2,
                  positronMaxRange);
    Decay(photon_number, time, src_id, position);
}

void PositronDecay::Decay(int photon_number, double time, int src_id,
                          VectorR3 position, double positronFWHM,
                          double positronMaxRange)
{
    // We set the gamma position here, prior to the positron range funtion,
    // because we assume the gamma is emitted from the nucleus, rather than the
    // positron.
    yellow.pos = position;
    this->position = position;
    // And set a flag to make sure the position is not overridden by the next
    // Decay call.
    gamma_position_set = true;
    PositronDecay::PositronRange(position, positronFWHM, positronMaxRange);
    Decay(photon_number, time, src_id, position);
}

void PositronDecay::PositronRange(VectorR3 & p, double positronC,
                                  double positronK1, double positronK2,
                                  double positronMaxRange)
{
    // First generate a direction that the photon will be blurred
    VectorR3 positronDir;
    Random::UniformSphere(positronDir);
    double range = 0.0;
    double cp;
    // generate cprime which is the scales the dual exponential into a form
    // that allows it to be monte-carlo generated
    cp = (positronC)/(positronC+positronK1/positronK2*(1-positronC));
    do {

        if (Random::Uniform() < cp) {
            range = Random::Exponential(positronK1);
        } else {
            range = Random::Exponential(positronK2);
        }

    } while (range > positronMaxRange); // rejection test positron range
    range *= CONST_MM_TO_CM;

    positronDir *= range;
    p += positronDir;
}

void PositronDecay::PositronRange(VectorR3 & p, double positronFWHM,
                                  double positronMaxRange)
{
    // First generate a direction that the photon will be blurred
    VectorR3 positronDir;
    Random::UniformSphere(positronDir);
    double range = 0.0;
    // must return cm, sigma expressed in mm
    do {
        range = Random::Gaussian() * positronFWHM *
        CONST_FWHM_TO_SIGMA * CONST_MM_TO_CM;
    } while (range > positronMaxRange); // rejection test positron range

    positronDir *= range;

    p += positronDir;
}

