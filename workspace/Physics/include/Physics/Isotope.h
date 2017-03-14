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
    void SetTime (const double t);
    void SetPosition (const VectorR3 &pos);
    void SetId(long i);
    long GetId() const;
    const VectorR3 & GetPosition() const;
    virtual void Decay(unsigned int photon_number) = 0;
    virtual void Reset() = 0;
    virtual std::ostream & print_on(std::ostream &) const = 0;
    Photon NextPhoton();
    bool IsEmpty() const;
    int source_num;
    long id;
    double time;
protected:
    VectorR3 pos;
    double half_life;
    VectorR3 position;
    void AddPhoton(Photon &p);
    friend std::ostream& operator<< (std::ostream & os, const Isotope & i);
    std::stack<Photon> daughter;
};

#endif /* ISOTOPE_H */
