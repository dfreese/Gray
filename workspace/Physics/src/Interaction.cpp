#include <Physics/Interaction.h>
#include <math.h>
#include <Physics/GammaStats.h>
#include <Random/Random.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

Interaction::Interaction() :
    type(NO_INTERACTION),
    id(0),
    time(0),
    pos(0, 0, 0),
    energy(0),
    color(Photon::P_BLUE),
    src_id(0),
    mat_id(0),
    det_id(-1),
    error(false),
    scatter(false),
    sensitive_mat(false)
{
}

Interaction Interaction::NoInteraction() {
    return(Interaction());
}

Interaction Interaction::Photoelectric(const Photon & p,
                                       const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = PHOTOELECTRIC;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = p.energy;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.error = false;
    hit.scatter = false;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::XrayEscape(const Photon & p, double deposit,
                                 const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = XRAY_ESCAPE;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = deposit;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.error = false;
    hit.scatter = false;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::Compton(const Photon & p, double deposit,
                                 const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = COMPTON;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = deposit;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.error = false;
    // This is a phantom scatter flag, so only flag if the material isn't
    // sensitive.
    hit.scatter = !mat_gamma_prop.log_material;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction Interaction::Rayleigh(const Photon & p,
                                  const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = RAYLEIGH;
    hit.id = p.id;
    hit.time = p.time;
    hit.pos = p.pos;
    hit.energy = 0;
    hit.color = p.color;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.det_id;
    hit.error = false;
    // This is a phantom scatter flag, so only flag if the material isn't
    // sensitive.
    hit.scatter = !mat_gamma_prop.log_material;
    hit.sensitive_mat = mat_gamma_prop.log_material;
    return(hit);
}

Interaction::KleinNishina::KleinNishina() :
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
double Interaction::KleinNishina::dsigma(double theta, double energy_mev)
{
    double alpha = energy_mev / ENERGY_511;
    double cs = cos(theta);
    double ss = sin(theta);
    double h = 1. / (1. + alpha * (1. - cs));
    double sigma = ss * h * h * (h + 1./ h - ss * ss);
    return(sigma);
}

/*!
 * Use std::upper_bound to binary search the closest value above the given
 * energy and then linearly interpolate the actual value from the lookup table
 * created in the constructor in energy_idx and dsigmal_max_val.
 */
double Interaction::KleinNishina::dsigma_max(double energy_mev)
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
    return((1.0 - alpha) * dsigma_max_val[idx - 1] +
           alpha * dsigma_max_val[idx]);
}


/*!
 * Calculate the pdf over it's max for use in an accept/reject monte carlo.  If
 * this is greater than or equal to a random value [0,1] then the angle theta
 * should be accepted.
 */
double Interaction::KleinNishina::dsigma_over_max(double theta,
                                                  double energy_mev)
{
    return(dsigma(theta, energy_mev) / dsigma_max(energy_mev));
}

/*!
 * For a particular energy, sweep theta for dsigma from 0 to pi in 100 steps to
 * determine the max.  100 steps is adequate for less than 0.5% error.
 */
double Interaction::KleinNishina::find_max(double energy_mev)
{
    // dsigma value is always positive, zero is safe.
    double max_val = 0;
    for (double theta = 0; theta <= M_PI; theta += (M_PI/ 100)) {
        max_val = std::max(max_val, dsigma(theta, energy_mev));
    }
    return(max_val);
}


Interaction::KleinNishina Interaction::klein_nishina;

const double Interaction::si1_SOL = (1.0 / 29979245800.0);

double Interaction::RandomExponentialDistance(double mu) {
    double r = Random::Uniform();
    if (r > 0.0) {
        return(- log(r) / mu);
    } else {
        return(DBL_MAX);
    }
}

/*!
 * Takes an energy (MeV) and uses that to calculate if there was an interaction
 * or not by calling RandomExponentialDistance.  dist is modified to be the
 * random distance if an interaction occurs.
 */
bool Interaction::PhotonInteracts(double energy,
                                  double & dist,
                                  const GammaStats & mat_gamma_prop)
{
    if (!mat_gamma_prop.enable_interactions) {
        return(false);
    }
    double mu = mat_gamma_prop.GetMu(energy);
    double rand_dist = RandomExponentialDistance(mu);
    if (dist > rand_dist) {
        dist = rand_dist;
        return(true);
    } else {
        return(false);
    }
}

Interaction Interaction::GammaInteraction(
        Photon &photon, double dist, const GammaStats & mat_gamma_prop)
{
    if (!PhotonInteracts(photon.energy, dist, mat_gamma_prop)) {
        return(NoInteraction());
    }

    // move photon to interaction point
    photon.pos += (dist * photon.dir.Normalize());
    photon.time += (dist * si1_SOL);

    // test for Photoelectric interaction
    switch (InteractionType(photon, mat_gamma_prop)) {
    case PHOTOELECTRIC:
        return(Photoelectric(photon, mat_gamma_prop));
    case XRAY_ESCAPE:
        return(XrayEscape(photon, 0, mat_gamma_prop));
    case COMPTON:
        // perform compton kinematics
        double deposit;
        Klein_Nishina(photon, deposit);
        return(Compton(photon, deposit, mat_gamma_prop));
    case RAYLEIGH:
        Rayleigh(photon);
        return(Rayleigh(photon, mat_gamma_prop));
    default:
        cerr << "ERROR: Incorrect interaction\n";
        exit(0);
        break;
    }
}

Interaction::INTER_TYPE Interaction::InteractionType(
        Photon &p,
        const GammaStats & mat_gamma_prop)
{
    double pe, compton, rayleigh;
    mat_gamma_prop.GetInteractionProbs(p.energy, pe, compton, rayleigh);

    double rand = (pe + compton + rayleigh) * Random::Uniform();
    if (rand < pe) {
        if (XrayEscape(p, mat_gamma_prop)) {
            // TODO: Get x-ray escape physics working again
            // 		 Xray needs to deposit majority of energy, and send small x-ray off
            return XRAY_ESCAPE;
        } else {
            return PHOTOELECTRIC;
        }
    } else if (rand < (pe + compton)) {
        return COMPTON;
    } else {
        return RAYLEIGH;
    }
}

void Interaction::Klein_Nishina_Angle(double energy, double & theta,
                                      double & phi)
{
    /* Generate scattering angles - phi and theta */
    // Theta is the compton angle

    theta = M_PI * Random::Uniform();
    double r = Random::Uniform();

    while (klein_nishina.dsigma_over_max(theta, energy) < r) {
        theta = M_PI * Random::Uniform();
        r = Random::Uniform();
    }

    // phi is symmetric around a circle of 360 degrees
    phi = M_2_PI * Random::Uniform();
}

double Interaction::Klein_Nishina_Energy(double energy, double theta)
{
    return(energy / (1.0 + (energy / ENERGY_511) * (1. - cos(theta))));
}

void Interaction::Klein_Nishina(Photon &p, double & deposit)
{
    double theta, phi;
    Klein_Nishina_Angle(p.energy, theta, phi);
    // After collision the photon loses some energy to the electron
    deposit = p.energy;
    p.energy = Klein_Nishina_Energy(p.energy, theta);
    deposit -= p.energy;

    // Create rotation axis this is perpendicular to Y axis
    // to generate the scattering angle theta
    RotationMapR3 rotation;
    VectorR3 rot_axis = p.dir;
    VectorR3 UnitY;
    UnitY.SetUnitY();
    rot_axis *= UnitY;
    rot_axis.Normalize();

    // save direction for phi rotatation
    VectorR3 comp_dir = p.dir;

    // rotate incline
    rotation.Set(rot_axis, theta);
    rotation.Transform(&comp_dir);

    // rotate theta using original direction as axis
    p.dir.Normalize();
    rotation.Set(p.dir,phi);
    rotation.Transform(&comp_dir);

    // next direction is from compton scattering angle
    p.dir = comp_dir;

    // If the photon scatters on a non-detector, it is a scatter, checked
    // inside SetScatter
    p.SetScatter();
}

/*!
 * The pdf of rayleigh scattering as a function of theta, normalized to a max
 * of one so that it can be used with an accept/reject method.
 */
double Interaction::RayleighProbability(double theta) {
    double cs = cos(theta);
    return((1.0 - cs * cs) / 2.0);
}

void Interaction::Rayleigh(Photon &p)
{
    double theta = M_PI * Random::Uniform();
    while (RayleighProbability(theta) < Random::Uniform()) {
        theta = M_PI * Random::Uniform();
    }

    // phi is symmetric around a circle of 360 degrees
    double phi = M_2_PI * Random::Uniform();

    // Create rotation axis this is perpendicular to Y axis
    // to generate the scattering angle theta
    RotationMapR3 rotation;
    VectorR3 rot_axis = p.dir;
    VectorR3 UnitY;
    UnitY.SetUnitY();
    rot_axis *= UnitY;
    rot_axis.Normalize();

    // save direction for phi rotatation
    VectorR3 comp_dir = p.dir;

    // rotate incline
    rotation.Set(rot_axis, theta);
    rotation.Transform(&comp_dir);

    // rotate theta using original direction as axis
    p.dir.Normalize();
    rotation.Set(p.dir,phi);
    rotation.Transform(&comp_dir);

    // next direction is from scattering angle
    p.dir = comp_dir;
}

bool Interaction::XrayEscape(Photon &p, const GammaStats & mat_gamma_prop)
{
    int num_escape = mat_gamma_prop.GetNumEscape();
    double photon_energy = p.energy;
    double * xray_escape = mat_gamma_prop.GetXrayEscape();
    double * xray_escape_probability = mat_gamma_prop.GetXrayEscapeProb();
    double rand = Random::Uniform();

    if (num_escape == 0) {
        //cerr << "X-ray excape not defined\n";
        return false;
    } else if (num_escape == 1) {
        if (photon_energy > xray_escape[0]) {
            // Inner shell interaction
            if (Random::Uniform() < mat_gamma_prop.GetAugerProb(0)) {
                p.energy = xray_escape[0];
                return true;
            } else {
                // Auger electron
                return false;
            }
        } else {
            return false;
        }
    } else {
        if (photon_energy < xray_escape[num_escape-1]) {
            // photon energy too small, no escapes
            return false;
        }
        for (int i = 0; i < num_escape; i++) {
            if (rand < xray_escape_probability[i]) {
                // Inner shell interaction
                if (photon_energy <= xray_escape[i]) // complete absorption
                    // TODO: log deposit energy and emit characteristic x-ray
                {
                    return false;
                }
                // Perform Auger Electron Check
                if (Random::Uniform() < mat_gamma_prop.GetAugerProb(i)) {
                    p.energy = xray_escape[i];
                    return true;
                } else {
                    return false;
                }
            }
        }
        return false;
    }
}
