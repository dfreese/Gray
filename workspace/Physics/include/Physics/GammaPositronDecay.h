#ifndef GAMMAPOSITRONDECAY_H
#define GAMMAPOSITRONDECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class GammaPositronDecay : public NuclearDecay
{
public:
    GammaPositronDecay(double acolinearity_deg_fwhm,
                       double gamma_decay_energy_mev,
                       double positron_emis_prob);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    virtual void Decay(int photon_number, double time, int src_id,
                       VectorR3 position, double positronC,
                       double positronK1, double positronK2,
                       double positronMaxRange);
    virtual void Decay(int photon_number, double time, int src_id,
                       VectorR3 position, double positronFWHM,
                       double positronMaxRange);

protected:
    const double acolinearity;
    const double gamma_decay_energy;
    const double positron_emission_prob;
    Photon blue;
    Photon red;
    Photon yellow;
    bool gamma_position_set;
};

#endif
