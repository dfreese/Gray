/*
 *	InteractionList
 *
 *		Record history of interactions as they happen
 *		Output parsers will record these to output files
 *
 */

#ifndef INTERACTION_LIST_H
#define INTERACTION_LIST_H

#include <DataStructs/Array.h>
#include <Deposit.h>

class Photon;
class Positron;
class Isotope;
class GammaMaterial;

class InteractionList
{
public:
    InteractionList();
    ~InteractionList();
    bool isEmpty() const
    {
        return hits.IsEmpty();
    }
    Deposit & NextHit();
    void Reset();
    void HitPositron(const Positron &p, double deposit);
    void HitCompton(const Photon &p, double deposit, const GammaMaterial & mat);
    void HitPhotoelectric(const Photon &p, double deposit, const GammaMaterial & mat);
    void HitRayleigh(const Photon &p, const GammaMaterial & mat) { };
    Array<Deposit> hits;
    friend ostream& operator<< (ostream &os, const InteractionList & l);
protected:
    void AddHit(Deposit & d);
    Isotope * cur_Isotope;
    Photon * cur_Photon;
    Deposit EMPTY;
    Deposit hit;
};

#endif
