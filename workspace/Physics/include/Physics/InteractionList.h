/*
 *	InteractionList
 *
 *		Record history of interactions as they happen
 *		Output parsers will record these to output files
 *
 */

#ifndef INTERACTION_LIST_H
#define INTERACTION_LIST_H

#include <vector>
#include <ostream>

class Deposit;
class Photon;
class Positron;
class Isotope;
class GammaStats;

class InteractionList
{
public:
    InteractionList();
    void Reset();
    void HitPositron(const Positron &p, double deposit);
    void HitCompton(const Photon &p, double deposit, const GammaStats & mat_gamma_prop);
    void HitPhotoelectric(const Photon &p, double deposit, const GammaStats & mat_gamma_prop);
    void HitRayleigh(const Photon &p, const GammaStats & mat_gamma_prop);
    std::vector<Deposit> hits;
    friend std::ostream& operator<< (std::ostream &os,
                                     const InteractionList & l);
};

#endif
