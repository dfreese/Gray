#ifndef CSE167_POSITRONDECAY_H
#define CSE167_POSITRONDECAY_H

#include <Physics/NuclearDecay.h>
#include <Physics/Photon.h>

class PositronDecay : public NuclearDecay
{
public:
    PositronDecay();
    void SetPositronRange(double c, double e1, double e2, double max_range);
    void SetPositronRange(double gauss, double max_range);
    void SetAcolinearity(double theta);
    void PositronRange(VectorR3 & p);
    void ClearPositronRange()
    {
        positronRange = false;
    };

    virtual void SetPosition(const VectorR3 & p);
    virtual void Decay(unsigned int photon_number);
    virtual void Reset();
    virtual ostream& print_on( ostream& os ) const;
    virtual DECAY_TYPE GetType() const
    {
        return POSITRON;
    };

public:
    Photon blue;
    Photon red;

protected:
    double acolinearity;
    // Positron Range variables
    bool positronRange;
    bool positronRangeGaussian;
    bool positronRangeCusp;
    double positronFWHM; // expressed in meters
    double positronC;
    double positronK1;
    double positronK2;
    double positronMaxRange;

};

#endif
