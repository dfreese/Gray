#ifndef CSE167_POSITRONDECAY_H
#define CSE167_POSITRONDECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class PositronDecay : public NuclearDecay
{
public:
    PositronDecay(double acolinearity_deg_fwhm);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    virtual void Decay(int photon_number, double time, int src_id,
                       VectorR3 position, double positronC,
                       double positronK1, double positronK2,
                       double positronMaxRange);
    virtual void Decay(int photon_number, double time, int src_id,
                       VectorR3 position, double positronFWHM,
                       double positronMaxRange);
    static void PositronRange(VectorR3 & p, double positronC,
                              double positronK1, double positronK2,
                              double positronMaxRange);
    static void PositronRange(VectorR3 & p, double positronFWHM,
                              double positronMaxRange);

    static const double default_acolinearity;

protected:
    const double acolinearity;
    Photon blue;
    Photon red;
};

#endif
