#ifndef GAMMAPOSITRONDECAY_H
#define GAMMAPOSITRONDECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class PositronDecay : public NuclearDecay
{
public:
    PositronDecay();
    PositronDecay(double acolinearity_deg_fwhm,
                  double positron_emis_prob = 1.0,
                  double gamma_decay_energy_mev = 0);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    void Decay(int photon_number, double time, int src_id,
               const VectorR3 & position, const VectorR3 & anni_position);

    static const double default_acolinearity;

private:
    double acolinearity;
    double gamma_decay_energy;
    double positron_emission_prob;
    bool emit_gamma;
    // TODO: evaluate creating these versus having it as a part of the class.
    // This makes it impossible to parallelize the raytracing of the photons.
    Photon blue;
    Photon red;
    Photon yellow;
};

#endif
