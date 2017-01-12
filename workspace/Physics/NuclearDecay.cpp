#include "NuclearDecay.h"

const double CONST_ACOL = (0.47 / 180.0) * PI / 2.35482005;
const double CONST_FWHM_TO_SIGMA = (1.0)/(2.35482005);
const double CONST_MM_TO_CM = (0.1); // 10 mm per cm
NuclearDecay::NuclearDecay()
{
    // Use default angle of 0.487 deg FWHM
    // 2.35482005 * sigma = FWHM

    pos.SetZero();
    beamDecay = false;

    // 120keV positron energy for FDG
    energy = 0.0;
    material = NULL;

    Gaussian();
    gaussAccess = false;
}

inline double NuclearDecay::Random()
{
    return genrand();
}

void NuclearDecay::SetBeam(const VectorR3 &axis, const double angle)
{
    beam_axis = axis;
    beam_angle = angle;
    beamDecay = true;
}

void NuclearDecay::SetPosition(const VectorR3 & p)
{
    pos = p;
}

// Helper function to optimize access to Gaussian Random number generator
double NuclearDecay::getGaussian()
{
    if (gaussAccess == false) {
        gaussAccess = true;
        return g1;
    } else {
        double ret = g2;
        Gaussian();
        gaussAccess = false;
        return ret;
    }
}

// Generate two gaussians for every call
// 	use private variables to store them, and access later
void NuclearDecay::Gaussian()
{
    double x1, x2, w;

    do {
        x1 = 2.0 * Random() - 1.0;
        x2 = 2.0 * Random() - 1.0;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );

    w = sqrt( (-2.0 * log( w ) ) / w );
    g1 = x1 * w;
    g2 = x2 * w;
}

void NuclearDecay::RandomHemiCube(VectorR3 & p)
{
    double z1,z2;
    z1 = Random();
    z2 = Random();
    p.x = sqrt(z1*(2.0-z1))*cos(2.0*PI*z2);
    p.y = sqrt(z1*(2.0-z1))*sin(2.0*PI*z2);
    p.z = 1 - z1;
}

void NuclearDecay::UniformSphere(VectorR3 & p)
{

    double cost = 2.0*Random() - 1.0;
    double phi = PI2*Random();
    double sint = sqrt(1.0-cost*cost);
    p.x = sint * cos(phi);
    p.y = sint * sin(phi);
    p.z = cost;

    p.Normalize();

}
