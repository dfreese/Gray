#ifndef GAMMA_MATERIAL_H
#define GAMMA_MATERIAL_H

#include <vector>
#include "Gray/Graphics/Material.h"
#include "Gray/Physics/GammaStats.h"
#include "Gray/Physics/Interaction.h"

class Photon;

class GammaMaterial : public Material {
public:
    GammaMaterial() = default;
    GammaMaterial(
        int index, const std::string& name, bool sensitive, bool interactive,
        GammaStats stats);
    bool Distance(Photon& photon, double max_dist) const;
    Interaction::Type Interact(Photon& photon) const;
    void DisableRayleigh();

private:
    GammaStats properties;
};

#endif // GAMMA_MATERIAL_H