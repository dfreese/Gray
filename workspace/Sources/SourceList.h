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

#ifndef CSE167_SOURCELIST_H
#define CSE167_SOURCELIST_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>
#include <string>
#include <map>

#include "Source.h"
#include "../Physics/BackBack.h"

#define FMIN(X,Y) ( ( (X)<(Y) ) ? (X):(Y))
#define FMAX(X,Y) ( ( (X)>(Y) ) ? (X):(Y))

using namespace std;

// a microcurie is 37kevents/second
const double microCurie = 37.0e3;

class SourceList
{
public:
    SourceList();
    ~SourceList();
    Isotope * Decay();
    void AddSource(Source & s);
    void SetCurIsotope(const char * iso);
    double Random();
    double GetTime()
    {
        return curTime;
    };
    double GetTotalEvents(double time);
private:
    static const int MAX_REJECT_COUNTER = 100000;
    double curTime;
    int search(double e, int b_idx, int s_idx);
    double total_activity;
    vector <Source*> list;
    vector <Source*> neg_list;
    vector <double> prob;
    double CalculateTime();
    bool Inside(const VectorR3 & pos);
    double mean_time_between_events;
    unsigned int photon_number;
    ISOTOPE_TYPE curIsotope;
    BackBack isotope;
};

#endif
