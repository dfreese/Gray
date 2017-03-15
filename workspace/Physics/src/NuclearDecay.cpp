#include <Physics/NuclearDecay.h>
#include <Random/Random.h>

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

void NuclearDecay::SetTime(const double t)
{
    time = t;
};

void NuclearDecay::SetBeam(const VectorR3 &axis, const double angle)
{
    beam_axis = axis;
    beam_angle = angle;
    beamDecay = true;
}

int NuclearDecay::GetSourceNum() const
{
    return(source_num);
};

void NuclearDecay::SetPosition(const VectorR3 & p)
{
    pos = p;
}

Photon NuclearDecay::NextPhoton()
{
    Photon val = daughter.top();
    daughter.pop();
    return val;
}

bool NuclearDecay::IsEmpty() const
{
    return(daughter.empty());
}

void NuclearDecay::AddPhoton(Photon &p)
{
    daughter.push(p);
}

MaterialBase * NuclearDecay::GetMaterial()
{
    return material;
}

void NuclearDecay::SetMaterial(MaterialBase * mat)
{
    material = mat;
}

