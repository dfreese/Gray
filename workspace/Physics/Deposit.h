/*
 *	Deposit
 *
 *		Whenever energy is deposited in a material a Deposit or Hit is created
 *
 */

#ifndef DEPOSIT_H
#define DEPOSIT_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "Photon.h"
#include "../Random/mt19937.h"

using namespace std;

enum HIT_TYPE {I_POSITRON, I_COMPTON, I_RAYLEIGH, I_PHOTOELECTRIC, I_GAMMA_DECAY, I_BETA_DECAY, I_AUGER, I_XRAY_ESCAPE, I_EMPTY, I_DEBUG, I_ERROR};

class Deposit
{
public:
    Deposit();
    ~Deposit();
    void Reset();
    friend ostream& operator<< ( ostream& os, const Deposit& d );
    bool operator<(const Deposit &d2);
public:
    VectorR3 pos;
    double energy;
    double time;

    HIT_TYPE type;

    long id;
    int color;
    int src_id;
    int mat_id;
    int det_id;
};

#endif
