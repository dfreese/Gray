#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <map>
#include "../DataStructs/Stack.h"
#include "Photon.h"

class MaterialBase;

enum ISOTOPE_TYPE {ISO_ERROR,ISO_F18, ISO_IN110, ISO_ZR89, ISO_CU64, ISO_BACKBACK, ISO_BEAM};

class Isotope
{
public:
    Isotope();
    void SetTime (const double t)
    {
        time = t;
    }
    void SetPosition (const VectorR3 &pos)
    {
        position = pos;
    }
    void SetMaterial (MaterialBase * mat)
    {
        material = mat;
    }
    void SetId(long i)
    {
        id = i;
    }
    ISOTOPE_TYPE getType(const char * iso);
    Isotope * newIsotope(ISOTOPE_TYPE type);
    long GetId() const
    {
        return id;
    }
    MaterialBase * GetMaterial()
    {
        return material;
    }
    const VectorR3 & GetPosition() const
    {
        return position;
    }
    virtual void Decay(unsigned int photon_number) = 0;
    virtual void Reset() = 0;
    virtual ostream & print_on(ostream &) const = 0;
    Photon & NextPhoton();
    bool IsEmpty() const
    {
        return daughter.IsEmpty();
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
    Stack<Photon> daughter;
    MaterialBase * material;
    map<string, ISOTOPE_TYPE> mapIsotope;
};

inline Photon & Isotope::NextPhoton()
{
    if (daughter.IsEmpty()) {
        return EMPTY;
    }
    //cout << "Pop[" << daughter.Size() << "]:" << daughter.Top() << endl;
    return daughter.Pop();
}

inline void Isotope::AddPhoton(Photon &p)
{
    daughter.Push(p);
}

#endif /* ISOTOPE_H */
