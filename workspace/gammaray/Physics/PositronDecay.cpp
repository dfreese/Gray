#include <Physics/PositronDecay.h>
#include <stdio.h>
#include <math.h>
#include <Random/Random.h>

using namespace std;

namespace {
    const double CONST_FWHM_TO_SIGMA = (1.0)/(2.35482005);
}

PositronDecay::PositronDecay() :
    acolinearity(0),
    gamma_decay_energy(0),
    positron_emission_prob(1.0),
    emit_gamma(false)
{
}

PositronDecay::PositronDecay(double acolinearity_deg_fwhm,
                             double positron_emis_prob,
                             double gamma_decay_energy_mev) :
    acolinearity(acolinearity_deg_fwhm / 180.0 * M_PI * CONST_FWHM_TO_SIGMA),
    gamma_decay_energy(gamma_decay_energy_mev),
    positron_emission_prob(positron_emis_prob),
    emit_gamma(true)
{
    if (gamma_decay_energy_mev <= 0) {
        emit_gamma = false;
    }
}

void PositronDecay::Decay(int photon_number, double time, int src_id,
                          const VectorR3 & position)
{
    Decay(photon_number, time, src_id, position, position);
}

void PositronDecay::Decay(int photon_number, double time, int src_id,
                          const VectorR3 & position,
                          const VectorR3 & anni_position)
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
    this->position = anni_position;

    if (emit_gamma) {
        // TODO: log the positron annihilation and nuclear decay positions
        // separately
        yellow.pos = position;
        // TODO: correctly set the time on the gamma decay, based on the
        // lifetime of the intermediate decay state.
        yellow.time = time;
        yellow.energy = gamma_decay_energy;
        yellow.id = photon_number;
        yellow.det_id = -1;
        yellow.src_id = src_id;
        yellow.scatter_compton_detector = 0;
        yellow.scatter_compton_phantom = 0;
        yellow.scatter_rayleigh_detector = 0;
        yellow.scatter_rayleigh_phantom = 0;
        yellow.xray_flouresence = 0;
        yellow.color = Photon::P_YELLOW;
        AddPhoton(&yellow);
    }

    // Check to see if a Positron was emitted with the gamma or not.
    if (Random::Uniform() < positron_emission_prob) {
        blue.time = time;
        blue.pos = anni_position;
        blue.energy = ENERGY_511;
        blue.id = photon_number;
        blue.det_id = -1;
        blue.src_id = src_id;
        blue.scatter_compton_detector = 0;
        blue.scatter_compton_phantom = 0;
        blue.scatter_rayleigh_detector = 0;
        blue.scatter_rayleigh_phantom = 0;
        blue.xray_flouresence = 0;
        red = blue;

        blue.SetBlue();
        red.SetRed();

        Random::UniformSphere(blue.dir);
        Random::Acolinearity(blue.dir, red.dir, acolinearity);
        AddPhoton(&blue);
        AddPhoton(&red);
    }
}
