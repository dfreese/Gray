#ifndef GAMMA_MATERIAL_H
#define GAMMA_MATERIAL_H

#include <Graphics/Material.h>
#include <Physics/GammaStats.h>

class GammaMaterial : public Material, public GammaStats {
public:
    GammaMaterial() : Material(), GammaStats() {};
    GammaMaterial(
        const std::string & name, int index,
        double density, bool sensitive, std::vector<double> energy,
        std::vector<double> matten_comp, std::vector<double> matten_phot,
        std::vector<double> matten_rayl, std::vector<double> x,
        std::vector<double> form_factor, std::vector<double> scattering_func) :
            Material(name),
            GammaStats(name, index,
                       density, sensitive, energy, matten_comp, matten_phot,
                       matten_rayl, x, form_factor, scattering_func)
    {}

};

#endif // GAMMA_MATERIAL_H
