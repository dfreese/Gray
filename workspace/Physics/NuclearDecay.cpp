#include <NuclearDecay.h>
#include <Random.h>

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
