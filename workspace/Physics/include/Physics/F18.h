#ifndef F18_H
#define F18_H

#include <Physics/Positron.h>

class F18 : public Positron
{
public:
    F18();
    virtual void Decay(unsigned int photon_number);
    virtual void Reset();
    virtual std::ostream & print_on(std::ostream & os) const;
protected:
};

#endif /* F18_H */
