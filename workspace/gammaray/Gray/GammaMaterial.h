#ifndef GAMMA_MATERIAL_H
#define GAMMA_MATERIAL_H

#include <vector>
#include <Graphics/Material.h>
#include <Physics/GammaStats.h>
#include <Physics/Interaction.h>

class Photon;

class GammaMaterial : public Material {
public:
    GammaMaterial() = default;
    GammaMaterial(
        int index, const std::string& name, bool sensitive, bool interactive,
        double density, std::vector<double> energy,
        std::vector<double> matten_comp, std::vector<double> matten_phot,
        std::vector<double> matten_rayl, std::vector<double> x,
        std::vector<double> form_factor, std::vector<double> scattering_func);
    bool Distance(Photon& photon, double max_dist) const;
    Interaction::Type Interact(Photon& photon) const;
    void DisableRayleigh();

private:
    GammaStats properties;
};

#endif // GAMMA_MATERIAL_H
