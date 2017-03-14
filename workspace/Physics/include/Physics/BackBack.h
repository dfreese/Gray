#ifndef BackBack_H
#define BackBack_H

#include <Physics/Positron.h>

class BackBack : public Positron
{
public:
    BackBack();
    virtual void Decay(unsigned int photon_number);
    virtual void Reset();
    virtual std::ostream & print_on(std::ostream & os) const;
};

#endif /* BackBack_H */
