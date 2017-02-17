#include <Physics/Interaction.h>
#include <math.h>
#include <Physics/GammaStats.h>
#include <Random/Random.h>
#include <stdlib.h>


using namespace std;

const double Interaction::si1_SOL = (1.0 / 29979245800.0);

// determine interaction in along line in material
bool Interaction::GammaAttenuation(double &dist, double mu)
{
    bool interaction = false;

    double r = Random::Uniform();
    double int_dis;

    if(r > 0.0 ) {
        int_dis = - log(r) / mu;
    } else {
        int_dis = 1.e300;
    }

    if(dist > int_dis) {
        interaction = true;
        dist = int_dis;
    }
    return interaction;
}


// determine if photoelectric or compton interaction
//		perform compton kinematics if comption interaction
//		TODO: perform rayleigh kinematics if rayleigh interaction
INTER_TYPE Interaction::GammaInteraction(Photon &photon, double dist, const GammaStats & mat_gamma_prop)
{
    double mu = 0.0;
    double sigma = 0.0;
    double rayleigh = 0.0;
    const double MEV_TO_KEY = 1000.0;

    // TODO: Add Rayleigh physics
    rayleigh = -1.0;
    if (!mat_gamma_prop.enable_interactions) {
        return NO_INTERACTION;
    }

    // Get attenuation coefficient and sigma, the pe/compton ratio at photon energy
    // change MeV to KeV
    mat_gamma_prop.GetPE(MEV_TO_KEY*photon.energy, mu, sigma);

    // determine if there is an interaction inter material
    bool interaction = GammaAttenuation(dist, mu);
    // set distance to the distance to interaction if true

    if (interaction) { // TODO: Add RAYLEIGH SCATTERING
        // move photon to interaction point
        photon.pos += (dist * photon.dir.Normalize());
        photon.time += (dist * si1_SOL);
        double dsdom;

        // test for Photoelectric interaction
        switch (PE(sigma, mu, photon, mat_gamma_prop)) {
        case PHOTOELECTRIC:
            return PHOTOELECTRIC;
        case XRAY_ESCAPE:
            return XRAY_ESCAPE;
        case COMPTON:
            // get scatter properties
            dsdom = mat_gamma_prop.GetDsDom(photon.energy);
            // perform compton kinematics
            Klein_Nishina(dsdom, photon, mat_gamma_prop);
            // If the photon scatters on a non-detector, it is a scatter, checked inside SetScatter
            photon.SetScatter();
            return COMPTON;
        default:
            cerr << "ERROR: Incorrect interaction\n";
            exit(0);
            break;
        }
    } else {
        return NO_INTERACTION;
    }
}

INTER_TYPE Interaction::PE(double sigma, double mu, Photon &p, const GammaStats & mat_gamma_prop)
{
    double rand = Random::Uniform();
    // determine photofraction
    if (rand > sigma/mu) {
        Photon ptmp;
        ptmp = p;
        if (XrayEscape(p, mat_gamma_prop)) {
            // TODO: Get x-ray escape physics working again
            // 		 Xray needs to deposit majority of energy, and send small x-ray off
            return XRAY_ESCAPE;
        } else {
            return PHOTOELECTRIC;
        }
    } else {
        return COMPTON;
    }
}

void Interaction::Klein_Nishina(double dsdom, Photon &p, const GammaStats & mat_gamma_prop)
{
    //alpha = *energy / 511.;
    // alpha is defined as the ratio between 511keV and energy
    double alpha = p.energy / ENERGY_511;

    /* Generate scattering angles - phi and theta */
    // Theta is the compton angle

    double phi = 0.0;
    double theta = M_PI * Random::Uniform();
    double s = dsigma(theta,alpha);
    double r = Random::Uniform();

    while (s / dsdom < r) {
        theta = M_PI * Random::Uniform();
        s = dsigma(theta,alpha);
        r = Random::Uniform();
    }

    // phi is symmetric around a circle of 360 degrees
    phi = M_2_PI * Random::Uniform();

    // After collision the photon loses some energy to the electron
    double deposit = p.energy;
    p.energy = p.energy /(1.0 + alpha * (1. - cos(theta)));
    deposit = (deposit-p.energy);

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
}

double Interaction::dsigma(double theta, double alpha)
{
    double	cs;
    double  ss;
    double	h;
    double	sigma;

    cs = cos(theta);
    ss = sin(theta);

    h = 1. / (1. + alpha * (1. - cs));

    sigma = h * h;
    sigma = sigma * (h + 1./ h - ss * ss);
    sigma = 2. * M_PI * ss * sigma;

    return(sigma);
}

bool Interaction::XrayEscape(Photon &p, const GammaStats & mat_gamma_prop)
{
    int i;

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
        for (i = 0; i < num_escape; i++) {
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
