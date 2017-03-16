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

    position.SetZero();
    blue.SetBlue();
    red.SetRed();

    // 120keV positron energy for FDG
    energy = 0.120;
    acolinearity = CONST_ACOL;

    while (!daughter.empty()) {
        daughter.pop();
    }
}

void PositronDecay::Decay(int photon_number, double time, int src_id,
                          const VectorR3 & position)
{
    blue.Reset();
    red.Reset();
    this->decay_number = photon_number;
    this->time = time;
    this->src_id = src_id;
    this->position = position;

    blue.time = time;
    blue.pos = position;
    blue.energy = ENERGY_511;
    blue.id = photon_number;
    blue.det_id = -1;
    blue.src_id = src_id;
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

void PositronDecay::Decay(int photon_number, double time, int src_id,
                          VectorR3 position, double positronC,
                          double positronK1, double positronK2,
                          double positronMaxRange)
{
    PositronRange(position, positronC, positronK1, positronK2,
                  positronMaxRange);
    Decay(photon_number, time, src_id, position);
}

void PositronDecay::Decay(int photon_number, double time, int src_id,
                          VectorR3 position, double positronFWHM,
                          double positronMaxRange)
{
    PositronRange(position, positronFWHM, positronMaxRange);
    Decay(photon_number, time, src_id, position);
}

