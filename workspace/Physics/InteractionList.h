/*
 *	InteractionList
 *
 *		Record history of interactions as they happen
 *		Output parsers will record these to output files
 *
 */

#ifndef INTERACTION_LIST_H
#define INTERACTION_LIST_H

#include "../Graphics/MaterialBase.h"
#include "../DataStructs/Array.h"
#include "Photon.h"
#include "Positron.h"
#include "Deposit.h"

using namespace std;

class Isotope;

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
    void HitCompton(const Photon &p, double deposit, const MaterialBase & mat);
    void HitPhotoelectric(const Photon &p, double deposit, const MaterialBase & mat);
    void HitRayleigh(const Photon &p, const MaterialBase & mat) { };
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
