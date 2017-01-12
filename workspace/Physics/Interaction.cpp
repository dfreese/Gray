#include "Interaction.h"

Interaction::Interaction() {}

Interaction::~Interaction() {}

// replace with MersienTwister: DONE
inline double Interaction::Random()
{
    return genrand();
    //return rand()/(double)RAND_MAX;
}

// determine interaction in along line in material
bool Interaction::GammaAttenuation(double &dist, double mu)
{
    bool interaction = false;

    double r = Random();
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
INTER_TYPE Interaction::GammaInteraction(Photon &photon, double dist, const MaterialBase & mat, InteractionList &l, Output &o)
{
    double mu = 0.0;
    double sigma = 0.0;
    double rayleigh = 0.0;
    const double MEV_TO_KEY = 1000.0;

    // TODO: Add Rayleigh physics
    rayleigh = -1.0;
    if (!mat.GammaProp->enable_interactions) {
        return NO_INTERACTION;
    }

    // Get attenuation coefficient and sigma, the pe/compton ratio at photon energy
    // change MeV to KeV
    mat.GammaProp->GetPE(MEV_TO_KEY*photon.energy, mu, sigma);

    // determine if there is an interaction inter material
    bool interaction = GammaAttenuation(dist, mu);
    // set distance to the distance to interaction if true

    if (interaction) { // TODO: Add RAYLEIGH SCATTERING
        // move photon to interaction point
        photon.pos += (dist * photon.dir.Normalize());
        photon.time += (dist * si1_SOL);
        double dsdom;

        // test for Photoelectric interaction
        switch (PE(sigma, mu, photon, mat, l, o)) {
        case PHOTOELECTRIC:
            return PHOTOELECTRIC;
        case XRAY_ESCAPE:
            return XRAY_ESCAPE;
        case COMPTON:
            // get scatter properties
            dsdom = mat.GammaProp->GetDsDom(photon.energy);
            // perform compton kinematics
            Klein_Nishina(dsdom, photon, mat, l, o);
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

INTER_TYPE Interaction::PE(double sigma, double mu, Photon &p, const MaterialBase & mat, InteractionList &l, Output &o)
{
    double rand = Random();
    // determine photofraction
    if (rand > sigma/mu) {
        Photon ptmp;
        ptmp = p;
        if (XrayEscape(p,mat, l, o)) {
            // TODO: Get x-ray escape physics working again
            // 		 Xray needs to deposit majority of energy, and send small x-ray off
            return XRAY_ESCAPE;
        } else {
            o.LogPhotoElectric(p,mat);
            l.HitPhotoelectric(p,p.energy, mat);
            return PHOTOELECTRIC;
        }
    } else {
        return COMPTON;
    }
}

void Interaction::Klein_Nishina(double dsdom, Photon &p, const MaterialBase & mat, InteractionList &l, Output &o)
{
    //alpha = *energy / 511.;
    // alpha is defined as the ratio between 511keV and energy
    double alpha = p.energy / ENERGY_511;

    /* Generate scattering angles - phi and theta */
    // Theta is the compton angle

    double phi = 0.0;
    double theta = PI * Random();
    double s = dsigma(theta,alpha);
    double r = Random();

    while (s / dsdom < r) {
        theta = PI*Random();
        s = dsigma(theta,alpha);
        r = Random();
    }

    // phi is symmetric around a circle of 360 degrees
    phi = PI2 * Random();

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

    // log interaction to file
    o.LogCompton(p, deposit, mat);
    l.HitCompton(p, deposit, mat);

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
    sigma = 2. * PI * ss * sigma;

    return(sigma);
}


void Interaction::UniformSphere(VectorR3 & p)
{

    double theta = PI2 * Random();
    double phi = PI2 * Random();
    double sinp = sin(phi);

    p.x = sinp * cos(theta);
    p.y = sinp * sin(theta);
    p.z = cos(phi);
}

bool Interaction::XrayEscape(Photon &p, const MaterialBase & mat, InteractionList &l, Output &o)
{
    int i;

    int num_escape = mat.GammaProp->GetNumEscape();
    double photon_energy = p.energy;
    double * xray_escape = mat.GammaProp->GetXrayEscape();
    double * xray_escape_probability = mat.GammaProp->GetXrayEscapeProb();
    double rand = Random();

    if (num_escape == 0) {
        //cerr << "X-ray excape not defined\n";
        return false;
    } else if (num_escape == 1) {
        if (photon_energy > xray_escape[0]) {
            // Inner shell interaction
            if (Random() < mat.GammaProp->GetAugerProb(0)) {
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
                if (Random() < mat.GammaProp->GetAugerProb(i)) {
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
