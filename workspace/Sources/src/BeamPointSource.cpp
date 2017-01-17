#include <Sources/BeamPointSource.h>
#include <Physics/Beam.h>

BeamPointSource::BeamPointSource(const VectorR3 &p, const VectorR3 &a, double angle, double act)
{
    position = p;
    SetActivity(act);
    beam_axis = a;
    // convert Angle FWHM to radians sigma
    beam_angle = (angle/180.0)*PI/2.35482005;
}

void BeamPointSource::Decay(unsigned int photon_number)
{
    if (isotope == NULL) {
        return;
    }

    //TODO: Fix beams!
    //SetBeam(beam_axis, beam_angle);
    ((Beam*)isotope)->SetBeam(beam_axis, beam_angle);
    isotope->SetPosition(position);
    isotope->SetMaterial(GetMaterial());
    isotope->Decay(photon_number);
}

bool BeamPointSource::Inside(const VectorR3 & pos) const
{
    return false;
}
