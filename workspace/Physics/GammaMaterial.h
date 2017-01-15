#ifndef GAMMA_MATERIAL_H
#define GAMMA_MATERIAL_H

#include <Material.h>
#include <GammaStats.h>

class GammaMaterial : public Material {
public:
    GammaMaterial() : Material() {}
    GammaStats GammaProp;
};

#endif // GAMMA_MATERIAL_H
