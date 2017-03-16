#include <Physics/PositronDecay.h>
#include <stdio.h>
#include <math.h>
#include <Random/Random.h>

using namespace std;

const double CONST_ACOL = (0.47 / 180.0) * M_PI / 2.35482005;
const double CONST_FWHM_TO_SIGMA = (1.0)/(2.35482005);
const double CONST_MM_TO_CM = (0.1); // 10 mm per cm
PositronDecay::PositronDecay()
{
    Reset();
}

void PositronDecay::Reset()
{
    // Use default angle of 0.487 deg FWHM
    // 2.35482005 * sigma = FWHM

    pos.SetZero();
    blue.SetBlue();
    red.SetRed();

    // 120keV positron energy for FDG
    energy = 0.120;
    acolinearity = CONST_ACOL;

    while (!daughter.empty()) {
        daughter.pop();
    }
}

void PositronDecay::Decay(int photon_number)
{
    blue.Reset();
    red.Reset();

    decay_number = photon_number;

    blue.time = time;
    blue.pos = pos;
    blue.energy = ENERGY_511;
    blue.id = photon_number;
    blue.det_id = -1;
    blue.src_id =  source_num;
    red = blue;

    blue.SetBlue();
    red.SetRed();

    blue.dir.SetUnitZ();
    Random::UniformSphere(blue.dir);
    Random::Acolinearity(blue.dir, red.dir, acolinearity);
    // clear beamDecay for next photon
    AddPhoton(&blue);
    AddPhoton(&red);
}

void PositronDecay::SetAcolinearity(double theta)
{
    acolinearity = theta;
}

void PositronDecay::SetPosition(const VectorR3 & p)
{
    pos = p;
    red.pos = p;
    blue.pos = p;
}

void PositronDecay::PositronRange(VectorR3 & p, double positronC,
                                  double positronK1, double positronK2,
                                  double positronMaxRange)
{
    // First generate a direction that the photon will be blurred
    VectorR3 positronDir;
    Random::UniformSphere(positronDir);
    double range = 0.0;
    double cp;
    // generate cprime which is the scales the dual exponential into a form that allows it
    // to be monte-carlo generated
    //
    cp = (positronC)/(positronC+positronK1/positronK2*(1-positronC));
    //		cout << "PositronRange: " << cp << " " << positronK1 << " " << positronK2;

    do {

        if (Random::Uniform() < cp) {
            range = Random::Exponential(positronK1);
        } else {
            range = Random::Exponential(positronK2);
        }

    } while (range > positronMaxRange); // rejection test positron range
    range *= CONST_MM_TO_CM;

    positronDir *= range;
    p += positronDir;
}

void PositronDecay::PositronRange(VectorR3 & p, double positronFWHM,
                                  double positronMaxRange)
{
    // First generate a direction that the photon will be blurred
    VectorR3 positronDir;
    Random::UniformSphere(positronDir);
    double range = 0.0;
    // must return cm, sigma expressed in mm
    do {
        range = Random::Gaussian() * positronFWHM * CONST_FWHM_TO_SIGMA * CONST_MM_TO_CM;
    } while (range > positronMaxRange); // rejection test positron range

    positronDir *= range;

    p += positronDir;
}

void PositronDecay::PositronRange(double positronC, double positronK1,
                                  double positronK2, double positronMaxRange)
{
    PositronRange(pos, positronC, positronK1, positronK2, positronMaxRange);
}

void PositronDecay::PositronRange(double positronFWHM, double positronMaxRange)
{
    PositronRange(pos, positronFWHM, positronMaxRange);
}

