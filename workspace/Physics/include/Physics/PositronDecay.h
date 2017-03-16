#ifndef CSE167_POSITRONDECAY_H
#define CSE167_POSITRONDECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class PositronDecay : public NuclearDecay
{
public:
    PositronDecay();
    void SetAcolinearity(double theta);
    virtual void SetPosition(const VectorR3 & p);
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    virtual void Reset();
    static void PositronRange(VectorR3 & p, double positronC,
                              double positronK1, double positronK2,
                              double positronMaxRange);
    static void PositronRange(VectorR3 & p, double positronFWHM,
                              double positronMaxRange);

    void PositronRange(double positronC, double positronK1, double positronK2,
                       double positronMaxRange);
    void PositronRange(double positronFWHM, double positronMaxRange);

private:
    Photon blue;
    Photon red;

protected:
    double acolinearity;
};

#endif
