/*
 * SimpleNurbs.h
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 */

#ifndef CSE167_PHOTON_H
#define CSE167_PHOTON_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>



















#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"

using namespace std;

#define ENERGY_511 0.510998903;
enum INTER_TYPE {COMPTON, PHOTOELECTRIC, AUGER, XRAY_ESCAPE, RAYLEIGH, INTERACTION, NO_INTERACTION, ESCAPE_INTERACTION,ERROR};

const int P_BLUE = 0;
const int P_RED = 1;
const int P_YELLOW = 2;

class Photon
{
public:
    Photon();
    ~Photon();
    //Photon(double e, const VectorR3 & p, const VectorR3 & d);
    Photon(int set_id, double e, const VectorR3 & p, const VectorR3 & d);
    void SetRed()
    {
        color = P_RED;
    }
    void SetBlue()
    {
        color = P_BLUE;
    }
    void SetSrc(int val)
    {
        src_id=val;
    }
    int GetSrc(void) const
    {
        return src_id;
    }
    void Reset();
    void SetScatter();
    bool CheckScatter() const
    {
        return phantom_scatter;
    }
    friend ostream& operator<< ( ostream& os, const Photon& p );
    friend bool operator==(const Photon &p1, const Photon &p2);
    Photon& operator=(const Photon &p2);
    bool operator<(const Photon &p2);

public:
    VectorR3 pos;
    VectorR3 dir;
    double energy;
    double time;
    int id;
    int color;
    int det_id;
    bool phantom_scatter;
    int src_id;
};

void inline Photon::SetScatter()
{
    //cout << "Scatter:["<<det_id<<"][" << phantom_scatter << "]\n";
    if ((det_id == -1) ) {
        phantom_scatter = true;
    }
}

#endif
