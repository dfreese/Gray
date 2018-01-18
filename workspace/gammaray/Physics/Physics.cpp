#include <Physics/Physics.h>
#include <cmath>
#include <Math/Math.h>
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
    hit.decay_id = p.GetId();
    hit.time = p.GetTime();
    hit.pos = p.GetPos();
    hit.energy = p.GetEnergy();
    hit.color = p.GetColor();
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.GetDetId();
    hit.scatter_compton_phantom = p.GetScatterComptonPhantom();
    hit.scatter_compton_detector = p.GetScatterComptonDetector();
    hit.scatter_rayleigh_phantom = p.GetScatterRayleighPhantom();
    hit.scatter_rayleigh_detector = p.GetScatterRayleighDetector();
    hit.xray_flouresence = p.GetXrayFlouresence();
    // Don't process this in the DAQ
    hit.dropped = true;
    return(hit);
}

Interaction Physics::Photoelectric(const Photon & p,
                                       const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = PHOTOELECTRIC;
    hit.decay_id = p.GetId();
    hit.time = p.GetTime();
    hit.pos = p.GetPos();
    hit.energy = p.GetEnergy();
    hit.color = p.GetColor();
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.GetDetId();
    hit.scatter_compton_phantom = p.GetScatterComptonPhantom();
    hit.scatter_compton_detector = p.GetScatterComptonDetector();
    hit.scatter_rayleigh_phantom = p.GetScatterRayleighPhantom();
    hit.scatter_rayleigh_detector = p.GetScatterRayleighDetector();
    hit.xray_flouresence = p.GetXrayFlouresence();
    // Process this in the DAQ if it was a sensitive interaction
    hit.dropped = !mat_gamma_prop.LogMaterial();
    return(hit);
}

Interaction Physics::XrayEscape(const Photon & p, double deposit,
                                 const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = XRAY_ESCAPE;
    hit.decay_id = p.GetId();
    hit.time = p.GetTime();
    hit.pos = p.GetPos();
    hit.energy = deposit;
    hit.color = p.GetColor();
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.GetDetId();
    hit.scatter_compton_phantom = p.GetScatterComptonPhantom();
    hit.scatter_compton_detector = p.GetScatterComptonDetector();
    hit.scatter_rayleigh_phantom = p.GetScatterRayleighPhantom();
    hit.scatter_rayleigh_detector = p.GetScatterRayleighDetector();
    hit.xray_flouresence = p.GetXrayFlouresence();
    // Process this in the DAQ if it was a sensitive interaction
    hit.dropped = !mat_gamma_prop.LogMaterial();
    return(hit);
}

Interaction Physics::Compton(const Photon & p, double deposit,
                                 const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = COMPTON;
    hit.decay_id = p.GetId();
    hit.time = p.GetTime();
    hit.pos = p.GetPos();
    hit.energy = deposit;
    hit.color = p.GetColor();
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.GetDetId();
    hit.scatter_compton_phantom = p.GetScatterComptonPhantom();
    hit.scatter_compton_detector = p.GetScatterComptonDetector();
    hit.scatter_rayleigh_phantom = p.GetScatterRayleighPhantom();
    hit.scatter_rayleigh_detector = p.GetScatterRayleighDetector();
    hit.xray_flouresence = p.GetXrayFlouresence();
    // Process this in the DAQ if it was a sensitive interaction
    hit.dropped = !mat_gamma_prop.LogMaterial();
    return(hit);
}

Interaction Physics::Rayleigh(const Photon & p,
                              const GammaStats & mat_gamma_prop)
{
    Interaction hit;
    hit.type = RAYLEIGH;
    hit.decay_id = p.GetId();
    hit.time = p.GetTime();
    hit.pos = p.GetPos();
    hit.energy = 0;
    hit.color = p.GetColor();
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.GetDetId();
    hit.scatter_compton_phantom = p.GetScatterComptonPhantom();
    hit.scatter_compton_detector = p.GetScatterComptonDetector();
    hit.scatter_rayleigh_phantom = p.GetScatterRayleighPhantom();
    hit.scatter_rayleigh_detector = p.GetScatterRayleighDetector();
    hit.xray_flouresence = p.GetXrayFlouresence();
    // Always a zero energy interaction, so don't process this in the DAQ
    hit.dropped = true;
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
    hit.decay_id = p.GetId();
    hit.time = p.GetTime();
    hit.pos = p.GetPos();
    hit.energy = p.GetEnergy();
    hit.color = p.GetColor();
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hit.det_id = p.GetDetId();
    hit.scatter_compton_phantom = p.GetScatterComptonPhantom();
    hit.scatter_compton_detector = p.GetScatterComptonDetector();
    hit.scatter_rayleigh_phantom = p.GetScatterRayleighPhantom();
    hit.scatter_rayleigh_detector = p.GetScatterRayleighDetector();
    hit.xray_flouresence = p.GetXrayFlouresence();
    // Don't process this in the DAQ
    hit.dropped = true;
    return(hit);
}

Interaction Physics::ErrorEmtpy(const Photon & p)
{
    Interaction hit;
    hit.type = ERROR_TRACE_DEPTH;
    hit.decay_id = p.GetId();
    hit.time = p.GetTime();
    hit.pos = p.GetPos();
    hit.energy = p.GetEnergy();
    hit.color = p.GetColor();
    hit.src_id = p.GetSrc();
    hit.mat_id = -1;
    hit.det_id = p.GetDetId();
    hit.scatter_compton_phantom = p.GetScatterComptonPhantom();
    hit.scatter_compton_detector = p.GetScatterComptonDetector();
    hit.scatter_rayleigh_phantom = p.GetScatterRayleighPhantom();
    hit.scatter_rayleigh_detector = p.GetScatterRayleighDetector();
    hit.xray_flouresence = p.GetXrayFlouresence();
    // Don't process this in the DAQ
    hit.dropped = true;
    return(hit);
}

/*!
 * Takes an energy (MeV) and uses that to calculate if there was an interaction
 * or not by calling RandomExponentialDistance.  dist is modified to be the
 * random distance if an interaction occurs.
 */
Physics::INTER_TYPE Physics::InteractionType(
        Photon & photon,
        double dist,
        const GammaStats & mat_gamma_prop,
        double & deposit)
{
    if (!mat_gamma_prop.InteractionsEnabled()) {
        // move photon to interaction point, or exit point of material
        photon.AddPos(dist * photon.GetDir());
        photon.AddTime(dist * inverse_speed_of_light);
        return(NO_INTERACTION);
    }

    double pe, compton, rayleigh;
    mat_gamma_prop.GetInteractionProbs(photon.GetEnergy(), pe, compton, rayleigh);

    double rand_dist = Random::Exponential(pe + compton + rayleigh);
    if (dist > rand_dist) {
        dist = rand_dist;
    }

    // move photon to interaction point, or exit point of material
    photon.AddPos(dist * photon.GetDir());
    photon.AddTime(dist * inverse_speed_of_light);
    if (dist < rand_dist) {
        return(NO_INTERACTION);
    }

    double rand = (pe + compton + rayleigh) * Random::Uniform();
    if (rand <= pe) {
        return PHOTOELECTRIC;
    } else if (rand <= (pe + compton)) {
        // perform compton kinematics
        ComptonScatter(photon, deposit, mat_gamma_prop);
        return COMPTON;
    } else {
        // perform rayleigh kinematics
        RayleighScatter(photon, mat_gamma_prop);
        return RAYLEIGH;
    }
}

double Physics::KleinNishinaEnergy(const double energy, const double costheta)
{
    return(energy / (1.0 + (energy / Physics::energy_511) * (1. - costheta)));
}

void Physics::ComptonScatter(Photon &p, double & deposit,
                             const GammaStats & mat_prop)
{
    const double costheta = mat_prop.GetComptonScatterAngle(p.GetEnergy());
    // After collision the photon loses some energy to the electron
    deposit = p.GetEnergy();
    p.SetEnergy(KleinNishinaEnergy(p.GetEnergy(), costheta));
    deposit -= p.GetEnergy();
    p.SetDir(Random::Deflection(p.GetDir(),costheta));
    p.SetScatterCompton();
}

void Physics::RayleighScatter(Photon &p, const GammaStats & mat_prop) {
    const double costheta = mat_prop.GetRayleighScatterAngle(p.GetEnergy());
    p.SetDir(Random::Deflection(p.GetDir(), costheta));
    // If the photon scatters on a non-detector, it is a scatter, checked
    // inside SetScatter
    p.SetScatterRayleigh();
}
