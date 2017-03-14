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
    beamDecay = false;

    // 120keV positron energy for FDG
    energy = 0.120;
    material = NULL;

    acolinearity = CONST_ACOL;

    positronRange = false;
    positronRangeGaussian = false;
    positronRangeCusp = false;

    positronFWHM = -1.0;
    positronC = -1.0;
    positronK1 = -1.0;
    positronK2 = -1.0;
    positronMaxRange = -1.0;
}

void PositronDecay::Decay(unsigned int photon_number)
{

    blue.Reset();
    red.Reset();

    if (positronRange) {
        PositronRange(pos);
    }

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
    if (beamDecay == false) {
        Random::UniformSphere(blue.dir);
        Random::Acolinearity(blue.dir, red.dir, acolinearity);
    } else { // if (beamDecay == true) {
        Random::Acolinearity(beam_axis, blue.dir, beam_angle);
        red.dir = blue.dir;
        red.dir.Negate();
        beamDecay = false;
    }
    // clear beamDecay for next photon
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

void PositronDecay::SetPositronRange(double C, double e1, double e2, double max_range)
{
    positronRange = true;
    positronRangeCusp = true;
    positronRangeGaussian = false;
    positronFWHM = -1.0;
    positronC = C; // dimensionless
    positronK1 = e1; // expressed in mm^-1
    positronK2 = e2; // expressed in mm^-1
    positronMaxRange = max_range; // expressed in meters
}

void PositronDecay::SetPositronRange(double gauss, double max_range)
{
    positronRange = true;
    positronRangeCusp = false;
    positronRangeGaussian = true;
    positronFWHM = gauss; 	// 100 cm / meter, range expressed in meters
    positronC = -1.0;
    positronK1 = -1.0;
    positronK2 = -1.0;
    positronMaxRange = max_range;	// expressed in meters
}

ostream& PositronDecay::print_on( ostream& os ) const
{
    char str[256];

    os << decay_number;
    os << " ";
    os << source_num;
    os << " ";
    sprintf(str,"%23.16e ",time);
    os << str;
    sprintf(str,"%12.6e ",energy);
    os << str;
    // positron is a first interaction
    sprintf(str,"%15.8e %15.8e %15.8e ",pos.x, pos.y, pos.z);
    os << str;

    return os;
}

/*******************************************************************************
 *            18F             11C            13N               15O             *
 *  C    0.519 (0.516)   0.501 (0.488)    0.433 (0.426)     0.263 (0.379)      *
 *  k1   27.9 (37.9)     24.5 (23.8)      25.4 (20.2)       33.2 (18.1)        *
 *  k2   2.91 (3.1)      1.76 (1.8)       1.44 (1.4)        1.0 (0.9)          *
 *******************************************************************************/

void PositronDecay::PositronRange(VectorR3 & p)
{
    // First generate a direction that the photon will be blurred
    VectorR3 positronDir;
    Random::UniformSphere(positronDir);
    double range = 0.0;

    if (positronRangeCusp == true) {
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

        // convert the range from mm to cm
        range *= CONST_MM_TO_CM;

        //cout << " Range (cm): " << range;

    } else if (positronRangeGaussian == true) {
        // must return cm, sigma expressed in mm
        do {
            range = Random::Gaussian() * positronFWHM * CONST_FWHM_TO_SIGMA * CONST_MM_TO_CM;
        } while (range > positronMaxRange); // rejection test positron range
    }

    positronDir *= range;

    p += positronDir;

    //cout << " PositionDir: " << positronDir.Norm() << endl;

    // TODO:  Set max range if there are errors in the photons?!
    // ie, use the geometery to determine the max_range
}
