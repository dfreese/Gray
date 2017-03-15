#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <map>
#include <stack>
#include <string>
#include <VrMath/LinearR3.h>
#include <Physics/Photon.h>

class Photon;

class Isotope
{
public:
    Isotope();
    virtual ~Isotope() {};
    virtual void Decay(unsigned int photon_number, double time,
                       const VectorR3 & position) = 0;
    virtual void Reset() = 0;
    virtual std::ostream & print_on(std::ostream &) const = 0;
    Photon NextPhoton();
    bool IsEmpty() const;
protected:
    double half_life;
    void AddPhoton(Photon &p);
    friend std::ostream& operator<< (std::ostream & os, const Isotope & i);
    std::stack<Photon> daughter;
};

#endif /* ISOTOPE_H */
