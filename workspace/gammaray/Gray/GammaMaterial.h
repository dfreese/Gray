#ifndef GAMMA_MATERIAL_H
#define GAMMA_MATERIAL_H

#include <Graphics/Material.h>
#include <Physics/GammaStats.h>

class GammaMaterial : public Material, public GammaStats {
public:
    GammaMaterial() : Material(), GammaStats() {}
};

#endif // GAMMA_MATERIAL_H
