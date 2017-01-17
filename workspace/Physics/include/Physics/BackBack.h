#ifndef BackBack_H
#define BackBack_H

#include "Positron.h"

class BackBack : public Positron
{
public:
    BackBack();
    virtual void Decay(unsigned int photon_number);
    virtual void Reset();
    virtual ostream & print_on(ostream & os) const;
};

#endif /* BackBack_H */
