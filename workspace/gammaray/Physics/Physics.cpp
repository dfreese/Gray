#include <Physics/Physics.h>
#include <cmath>
#include <Physics/GammaStats.h>
#include <Physics/NuclearDecay.h>
#include <Random/Random.h>
#include <algorithm>

using namespace std;

Interaction Physics::NoInteraction() {
    return(Interaction());
}

Interaction Physics::NoInteraction(const Photon & p,
                                       const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = NO_INTERACTION;
    hit.decay_id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    // Don't process this in the DAQ
    hit.dropped = true;
    return(hit);
}

Interaction Physics::Photoelectric(const Photon & p,
                                       const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = PHOTOELECTRIC;
    hit.decay_id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    // Process this in the DAQ if it was a sensitive interaction
    hit.dropped = !mat_gamma_prop.log_material;
    return(hit);
}

Interaction Physics::XrayEscape(const Photon & p, double deposit,
                                 const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = XRAY_ESCAPE;
    hit.decay_id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = deposit;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    // Process this in the DAQ if it was a sensitive interaction
    hit.dropped = !mat_gamma_prop.log_material;
    return(hit);
}

Interaction Physics::Compton(const Photon & p, double deposit,
                                 const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = COMPTON;
    hit.decay_id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = deposit;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    // Process this in the DAQ if it was a sensitive interaction
    hit.dropped = !mat_gamma_prop.log_material;
    return(hit);
}

Interaction Physics::Rayleigh(const Photon & p,
                                  const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = RAYLEIGH;
    hit.decay_id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = 0;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    // Process this in the DAQ if it was a sensitive interaction
    hit.dropped = !mat_gamma_prop.log_material;
    return(hit);
}

Interaction Physics::NuclearDecay(const class NuclearDecay &p,
                                      const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = NUCLEAR_DECAY;
    hit.decay_id = p.GetDecayNumber();
    hit.time = p.GetTime();
    hit.pos = p.GetPosition();
    hit.energy = p.GetEnergy();
    hit.color = Photon::P_YELLOW;
    hit.src_id = p.GetSourceId();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = -1;
    hit.scatter_compton_phantom = 0;
    hit.scatter_compton_detector = 0;
    hit.scatter_rayleigh_phantom = 0;
    hit.scatter_rayleigh_detector = 0;
    hit.xray_flouresence = 0;
    // Don't process this in the DAQ
    hit.dropped = true;
    return(hit);
}


Interaction Physics::ErrorTraceDepth(const Photon & p,
                                         const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = ERROR_TRACE_DEPTH;
    hit.decay_id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    // Don't process this in the DAQ
    hit.dropped = true;
    return(hit);
}

Interaction Physics::ErrorEmtpy(const Photon & p)
{
    Interaction hit;
    hit.type = ERROR_TRACE_DEPTH;
    hit.decay_id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = -1;
    hit.det_id = p.det_id;
    hit.scatter_compton_phantom = p.scatter_compton_phantom;
    hit.scatter_compton_detector = p.scatter_compton_detector;
    hit.scatter_rayleigh_phantom = p.scatter_rayleigh_phantom;
    hit.scatter_rayleigh_detector = p.scatter_rayleigh_detector;
    hit.xray_flouresence = p.xray_flouresence;
    // Don't process this in the DAQ
    hit.dropped = true;
    return(hit);
}

Physics::KleinNishina::KleinNishina() :
    // These energies were chosen, as they give less than 0.5% error from 0 to
    // 1.5MeV when linear interpolation is performed.
    energy_idx({
        0.0, 0.010, 0.030, 0.050, 0.100, 0.200, 0.300, 0.400, 0.500, 0.600,
        0.700, 0.900, 1.100, 1.300, 1.500})
{
    dsigma_max_val.resize(energy_idx.size());
    std::transform(energy_idx.begin(), energy_idx.end(),
                   dsigma_max_val.begin(), find_max);
}

/*!
 * Calculates dsigma / dtheta for the Klein-Nishina formula to be used in a
 * accept/reject monte carlo.  For this reason the constants at the front of the
 * formula have been dropped out, as they will be divided out eventually by the
 * max.
 * In this case h is P(E, theta) as seen here:
 * https://en.wikipedia.org/wiki/Klein–Nishina_formula
 *
 */
double Physics::KleinNishina::dsigma(double theta, double energy_mev,
                                         double & prob_e_theta)
{
    double alpha = energy_mev / Physics::energy_511;
    double cs = cos(theta);
    double ss = sin(theta);
    prob_e_theta = 1. / (1. + alpha * (1. - cs));
    double sigma = ss * prob_e_theta * prob_e_theta * (prob_e_theta +
                                                       (1./ prob_e_theta) -
                                                       ss * ss);
    return(sigma);
}

/*!
 * Use std::upper_bound to binary search the closest value above the given
 * energy and then linearly interpolate the actual value from the lookup table
 * created in the constructor in energy_idx and dsigmal_max_val.
 */
double Physics::KleinNishina::dsigma_max(double energy_mev)
{
    size_t idx = upper_bound(energy_idx.begin(), energy_idx.end(), energy_mev) -
                 energy_idx.begin();

    if (idx == 0) {
        return(dsigma_max_val.front());
    } else if (idx == energy_idx.size()) {
        return(dsigma_max_val.back());
    }
    double delta = energy_idx[idx] - energy_idx[idx - 1];
    double alpha = (energy_mev - energy_idx[idx - 1]) / delta;
    if (alpha > 1.0) {
        alpha = 1.0;
    }
    double dsigma_max = ((1.0 - alpha) * dsigma_max_val[idx - 1] +
                         alpha * dsigma_max_val[idx]);
    return(dsigma_max);
}


/*!
 * Calculate the pdf over it's max for use in an accept/reject monte carlo.  If
 * this is greater than or equal to a random value [0,1] then the angle theta
 * should be accepted.
 */
double Physics::KleinNishina::dsigma_over_max(double theta,
                                                  double energy_mev,
                                                  double & prob_e_theta)
{
    return(dsigma(theta, energy_mev, prob_e_theta) / dsigma_max(energy_mev));
}

/*!
 * For a particular energy, sweep theta for dsigma from 0 to pi in 100 steps to
 * determine the max.  100 steps is adequate for less than 0.5% error.
 */
double Physics::KleinNishina::find_max(double energy_mev)
{
    // dsigma value is always positive, zero is safe.
    double max_val = 0;
    for (double theta = 0; theta <= M_PI; theta += (M_PI/ 100)) {
        double prob_e_theta;
        max_val = std::max(max_val, dsigma(theta, energy_mev, prob_e_theta));
    }
    return(max_val);
}

Physics::KleinNishina Physics::klein_nishina;

/*!
 * Takes an energy (MeV) and uses that to calculate if there was an interaction
 * or not by calling RandomExponentialDistance.  dist is modified to be the
 * random distance if an interaction occurs.
 */
Physics::INTER_TYPE Physics::InteractionType(
        Photon & photon,
        double & dist,
        const GammaStats & mat_gamma_prop,
        double & deposit)
{
    if (!mat_gamma_prop.enable_interactions) {
        // move photon to interaction point, or exit point of material
        photon.pos += (dist * photon.dir.Normalize());
        photon.time += (dist * inverse_speed_of_light);
        return(NO_INTERACTION);
    }

    double pe, compton, rayleigh;
    mat_gamma_prop.GetInteractionProbs(photon.energy, pe, compton, rayleigh);

    // TODO: add back in rayleigh scattering once the distribution is fixed
    // double rand_dist = RandomExponentialDistance(pe + compton + rayleigh);
    double rand_dist = Random::Exponential(pe + compton);
    if (dist > rand_dist) {
        dist = rand_dist;
    }

    // move photon to interaction point, or exit point of material
    photon.pos += (dist * photon.dir.Normalize());
    photon.time += (dist * inverse_speed_of_light);
    if (dist < rand_dist || !mat_gamma_prop.enable_interactions) {
        return(NO_INTERACTION);
    }


    // TODO: add back in rayleigh scattering once the distribution is fixed
    // double rand = (pe + compton + rayleigh) * Random::Uniform();
    double rand = (pe + compton) * Random::Uniform();
    if (rand <= pe) {
        if (XrayEscape(photon, mat_gamma_prop, deposit)) {
            // TODO: Get x-ray escape physics working again
            return XRAY_ESCAPE;
        } else {
            return PHOTOELECTRIC;
        }
    } else if (rand <= (pe + compton)) {
        // perform compton kinematics
        ComptonScatter(photon, deposit);
        return COMPTON;
    } else {
        // perform rayleigh kinematics
        RayleighScatter(photon);
        return RAYLEIGH;
    }
}

/*!
 * A random angle theta based on the Klein-Nishina distribution given the
 * current energy.  Also returns the probability of that theta to be used for
 * the energy calculation later on.
 */
double Physics::KleinNishinaAngle(double energy, double & prob_e_theta)
{
    /* Generate scattering angles - phi and theta */
    // Theta is the compton angle
    double theta;
    do {
        theta = M_PI * Random::Uniform();
        // Continue to loop until we accept something
    } while (!Random::Selection(klein_nishina.dsigma_over_max(theta, energy,
                                                              prob_e_theta)));
    return (theta);
}

double Physics::KleinNishinaEnergy(double energy, double theta)
{
    return(energy / (1.0 + (energy / Physics::energy_511) * (1. - cos(theta))));
}

void Physics::ComptonScatter(Photon &p, double & deposit)
{
    double prob_e_theta;
    const double theta = KleinNishinaAngle(p.energy, prob_e_theta);
    // After collision the photon loses some energy to the electron
    deposit = p.energy * (1 - prob_e_theta);
    p.energy *= prob_e_theta;
    p.dir = Random::Deflection(p.dir, theta);
    p.SetScatterCompton();
}

/*!
 * The pdf of rayleigh scattering as a function of theta, normalized to a max
 * of one so that it can be used with an accept/reject method.
 */
double Physics::RayleighProbability(double theta) {
    double cs = cos(theta);
    return((1.0 - cs * cs) / 2.0);
}

/*!
 * Generates a random angle for Rayleigh Scattering based on an accept/reject
 * method using RayleighProbability.  Angle range is [0, pi].
 */
double Physics::RayleighAngle() {

    // FIXME: This implements Thompson scattering, not Rayleigh scattering
    double theta;
    do {
        theta = M_PI * Random::Uniform();
        // Keep generating an angle until we generate a select based on the
        // normalized pdf.
    } while (!Random::Selection(RayleighProbability(theta)));
    return (theta);
}

void Physics::RayleighScatter(Photon &p)
{
    const double theta = RayleighAngle();
    p.dir = Random::Deflection(p.dir, theta);
    // If the photon scatters on a non-detector, it is a scatter, checked
    // inside SetScatter
    p.SetScatterRayleigh();
}

bool Physics::XrayEscape(Photon &p, const GammaStats & mat_gamma_prop,
                             double & deposit)
{
    const std::vector<double> & emit_e = mat_gamma_prop.GetXrayEmissionEnergies();
    const std::vector<double> & prob_e = mat_gamma_prop.GetXrayEmissionCumProb();
    double rand = Random::Uniform();
    rand *= mat_gamma_prop.GetXrayBindEnergyScale(p.energy);
    size_t idx = lower_bound(prob_e.begin(), prob_e.end(), rand) - prob_e.begin();
    double xray_energy = emit_e[idx];
    if (xray_energy == 0) {
        return(false);
    } else {
        return(false);
        deposit = p.energy - xray_energy;
        p.energy = xray_energy;
        p.dir = Random::UniformSphere();
        p.SetXrayFlouresence();
        return(true);
    }
}
