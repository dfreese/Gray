#ifndef IN110_H
#define IN110_H

#include <Physics/Positron.h>
#include <Physics/GammaDecay.h>

class IN110 : public Positron
{
public:
    IN110();
    virtual void Decay(unsigned int photon_number);
    virtual void Reset();
    virtual ostream & print_on(ostream & os) const;
private:
    // TODO: Model time between Gamma and Positron Decay accurately
    GammaDecay g;
    const double CONST_E_IN110m_GAMMA = 0.657750; /* MeV */
    const double CONST_PROB_IN110m_POS = 0.61;	/* 61% of decays lead to a positron */

};

#endif /* IN110_H */
