#include "Gray/Gray/GammaMaterial.h"
#include <vector>
#include "Gray/Physics/GammaStats.h"
#include "Gray/Physics/Interaction.h"
#include "Gray/Physics/Photon.h"
#include "Gray/Physics/Physics.h"
#include "Gray/Random/Random.h"

GammaMaterial::GammaMaterial(
        int index, const std::string& name, bool sensitive, bool interactive,
        GammaStats stats) :
    Material(index, name, sensitive, interactive),
    properties(std::move(stats))
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

