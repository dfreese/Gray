#ifndef GAMMAPOSITRONDECAY_H
#define GAMMAPOSITRONDECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class PositronDecay : public NuclearDecay
{
public:
    PositronDecay();
    PositronDecay(double acolinearity_deg_fwhm);
    PositronDecay(double acolinearity_deg_fwhm,
                       double positron_emis_prob);
    PositronDecay(double acolinearity_deg_fwhm,
                       double positron_emis_prob,
                       double gamma_decay_energy_mev);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    void Decay(int photon_number, double time, int src_id,
                       VectorR3 position, double positronC,
                       double positronK1, double positronK2,
                       double positronMaxRange);
    void Decay(int photon_number, double time, int src_id,
                       VectorR3 position, double positronFWHM,
                       double positronMaxRange);
    static void PositronRange(VectorR3 & p, double positronC,
                              double positronK1, double positronK2,
                              double positronMaxRange);
    static void PositronRange(VectorR3 & p, double positronFWHM,
                              double positronMaxRange);
    void set_acolinearity(double acolinearity_deg_fwhm);

    static const double default_acolinearity;

protected:
    double acolinearity;
    double gamma_decay_energy;
    double positron_emission_prob;
    Photon blue;
    Photon red;
    Photon yellow;
    bool gamma_position_set;
    bool emit_gamma;
};

#endif
