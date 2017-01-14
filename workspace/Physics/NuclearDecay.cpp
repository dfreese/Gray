#include "NuclearDecay.h"
#include <Random.h>

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

    Random::Gaussian();
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
