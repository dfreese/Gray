#include <Gray/GammaMaterial.h>
#include <vector>
#include <Physics/GammaStats.h>
#include <Physics/Interaction.h>
#include <Physics/Photon.h>
#include <Physics/Physics.h>
#include <Random/Random.h>

GammaMaterial::GammaMaterial(
        int index, const std::string& name, bool sensitive, bool interactive,
        double density, std::vector<double> energy,
        std::vector<double> matten_comp, std::vector<double> matten_phot,
        std::vector<double> matten_rayl, std::vector<double> x,
        std::vector<double> form_factor, std::vector<double> scattering_func) :
    Material(index, name, sensitive, interactive),
    properties(density, energy, matten_comp, matten_phot,
            matten_rayl, x, form_factor, scattering_func)
{}

bool GammaMaterial::Distance(Photon& photon, double max_dist) const {
    double rand_dist = max_dist;
    bool interacted = false;
    if (InteractionsEnabled()) {
        GammaStats::AttenLengths len = properties.GetAttenLengths(
                photon.GetEnergy());
        rand_dist = Random::Exponential(len.total());
        interacted = true;
        if (rand_dist >= max_dist) {
            interacted = false;
            rand_dist = max_dist;
        }
    }

    // move photon to interaction point, or exit point of material
    photon.AddPos(rand_dist * photon.GetDir());
    photon.AddTime(rand_dist * Physics::inverse_speed_of_light);
    return (interacted);
}

Interaction::Type GammaMaterial::Interact(Photon& photon) const {
    GammaStats::AttenLengths len = properties.GetAttenLengths(photon.GetEnergy());
    double rand = len.total() * Random::Uniform();
    if (rand <= len.photoelectric) {
        photon.SetEnergy(0);
        return (Interaction::Type::PHOTOELECTRIC);
    } else if (rand <= (len.photoelectric + len.compton)) {
        // perform compton kinematics
        properties.ComptonScatter(photon);
        return (Interaction::Type::COMPTON);
    } else {
        // perform rayleigh kinematics
        properties.RayleighScatter(photon);
        return (Interaction::Type::RAYLEIGH);
    }
}

void GammaMaterial::DisableRayleigh() {
    properties.DisableRayleigh();
}

