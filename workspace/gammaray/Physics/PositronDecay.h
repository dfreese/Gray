#ifndef GAMMAPOSITRONDECAY_H
#define GAMMAPOSITRONDECAY_H

#include <Physics/NuclearDecay.h>

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
    bool EmitsGamma() const;
    double PositronEmissionProb() const;

private:
    double acolinearity;
    double gamma_decay_energy;
    double positron_emission_prob;
    bool emit_gamma;
};

#endif
