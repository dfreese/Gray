#ifndef IN110_H
#define IN110_H

#include "Positron.h"
#include "GammaDecay.h"

const double CONST_E_IN110m_GAMMA = 0.657750; /* MeV */
const double CONST_PROB_IN110m_POS = 0.61;	/* 61% of decays lead to a positron */

class IN110 : public Positron
{
public:
    IN110();
    virtual void Decay(unsigned int photon_number);
    virtual void Reset();
    virtual ostream & print_on(ostream & os) const;
    // TODO: Model time between Gamma and Positron Decay accurately
    GammaDecay g;
};

#endif /* IN110_H */
