#include <Physics/PositronDecay.h>
#include <Physics/Photon.h>
#include <Physics/Physics.h>
#include <Random/Random.h>
#include <Random/Transform.h>

using namespace std;

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
    acolinearity(acolinearity_deg_fwhm / 180.0 * M_PI * Transform::fwhm_to_sigma),
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
}

bool PositronDecay::EmitsGamma() const {
    return(emit_gamma);
}

double PositronDecay::PositronEmissionProb() const {
    return(positron_emission_prob);
}

double PositronDecay::GammaDecayEnergy() const {
    return (gamma_decay_energy);
}

double PositronDecay::Acolinearity() const {
    return (acolinearity);
}
