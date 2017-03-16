#ifndef F18_H
#define F18_H

#include <Physics/Positron.h>

class F18 : public Positron
{
public:
    F18();
    virtual void Decay(int photon_number, double time, int src_id,
                       const VectorR3 & position);
    virtual void Reset();
protected:
};

#endif /* F18_H */
