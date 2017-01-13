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

#ifndef CSE167_INTERACTION_H
#define CSE167_INTERACTION_H

// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>
#include <iostream>
#include <fstream>
//#include <iostream.h>  // I/O
//#include <fstream.h>   // file I/O

#include "../Graphics/MaterialBase.h"
#include "../Graphics/TransformViewable.h"
#include "../Graphics/VisiblePoint.h"
#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"
#include "Photon.h"
#include "PositronDecay.h"
#include "../Random/mt19937.h"
#include "../Output/Output.h"
#include "InteractionList.h"

using namespace std;

//#define XRAY_ESCAPE_ENERGY_LSO 63.314
//#define XRAY_ESCAPE_ENERGY_CD 26.711
//#define XRAY_ESCALE_ENERGY_TE 31.814
// TODO DO CZT XRAY_ESCAPE PHYSICS

const double si1_SOL = (1.0 / 29979245800.0);

class Interaction
{
public:
    Interaction();
    ~Interaction();
    INTER_TYPE GammaInteraction(Photon &p, double dist, const MaterialBase & mat, InteractionList &l, Output &o);
    bool GammaAttenuation(double &dist, double u_attentuation);
    INTER_TYPE PE(double sigma, double mu, Photon &p, const MaterialBase & mat, InteractionList &l, Output &o);
    void Klein_Nishina(double dsdom, Photon &p, const MaterialBase & mat, InteractionList &l, Output &o);
    double Random();
    double dsigma(double phi, double alpha);
private:
    bool XrayEscape(Photon &p, const MaterialBase & mat, InteractionList &l, Output &o);
    void UniformSphere(VectorR3 & p);
    VectorR3 comp_pos;
};

#endif
