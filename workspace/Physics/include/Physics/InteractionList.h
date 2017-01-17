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
#include <Physics/Deposit.h>

class Photon;
class Positron;
class Isotope;
class GammaStats;

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
    void HitCompton(const Photon &p, double deposit, const GammaStats & mat_gamma_prop);
    void HitPhotoelectric(const Photon &p, double deposit, const GammaStats & mat_gamma_prop);
    void HitRayleigh(const Photon &p, const GammaStats & mat_gamma_prop) { };
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
