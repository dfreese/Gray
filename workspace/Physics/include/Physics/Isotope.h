#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <map>
#include <stack>
#include <string>
#include <Physics/Photon.h>

class Isotope
{
public:
    Isotope();
    virtual ~Isotope() {};
    void SetTime (const double t)
    {
        time = t;
    }
    void SetPosition (const VectorR3 &pos)
    {
        position = pos;
    }
    void SetId(long i)
    {
        id = i;
    }
    long GetId() const
    {
        return id;
    }
    const VectorR3 & GetPosition() const
    {
        return position;
    }
    virtual void Decay(unsigned int photon_number) = 0;
    virtual void Reset() = 0;
    virtual ostream & print_on(ostream &) const = 0;
    Photon NextPhoton();
    bool IsEmpty() const
    {
        return daughter.empty();
    }
    int source_num;
public:
    long id;
    double time;
protected:
    VectorR3 pos;
    double half_life;
    VectorR3 position;
    void AddPhoton(Photon &p);
    inline friend ostream& operator<< (ostream&os,const Isotope &i)
    {
        return i.print_on(os);
    }
    Photon EMPTY;
    std::stack<Photon> daughter;
};

inline Photon Isotope::NextPhoton()
{
    if (daughter.empty()) {
        return EMPTY;
    }
    Photon val = daughter.top();
    daughter.pop();
    return val;
}

inline void Isotope::AddPhoton(Photon &p)
{
    daughter.push(p);
}

#endif /* ISOTOPE_H */
