#include <Physics/PositronDecay.h>
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

    if (emit_gamma) {
        // TODO: log the positron annihilation and nuclear decay positions
        // separately
        // TODO: correctly set the time on the gamma decay, based on the
        // lifetime of the intermediate decay state.
        yellow = Photon(position, Random::UniformSphere(), gamma_decay_energy,
                        time, photon_number, Photon::P_YELLOW, src_id);
        AddPhoton(&yellow);
    }

    // Check to see if a Positron was emitted with the gamma or not.
    if (Random::Selection(positron_emission_prob)) {
        blue = Photon(anni_position, Random::UniformSphere(), Physics::energy_511,
                      time, photon_number, Photon::P_BLUE, src_id);


        red = Photon(anni_position,
                     Random::Acolinearity(blue.GetDir(), acolinearity),
                     Physics::energy_511, time, photon_number, Photon::P_RED,
                     src_id);

        AddPhoton(&blue);
        AddPhoton(&red);
    }
}

bool PositronDecay::EmitsGamma() const {
    return(emit_gamma);
}


double PositronDecay::PositronEmissionProb() const {
    return(positron_emission_prob);
}
