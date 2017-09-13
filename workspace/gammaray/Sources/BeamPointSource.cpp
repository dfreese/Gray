#include <Sources/BeamPointSource.h>
#include <Physics/Beam.h>
#include <exception>

BeamPointSource::BeamPointSource(const VectorR3 &p, const VectorR3 &a, double angle, double act) :
    Source(p, act),
    beam_axis(a),
    // convert Angle FWHM to radians sigma
    beam_angle((angle/180.0) * M_PI/2.35482005)
{
}

void BeamPointSource::SetIsotope(Isotope * i)
{
    isotope = dynamic_cast<Beam*>(i);
    if (!isotope) {
        throw(std::runtime_error("BeamPointSource requires Beam Isotope"));
    }
};

VectorR3 BeamPointSource::Decay(int photon_number, double time)
{
    dynamic_cast<Beam*>(isotope)->SetBeam(beam_axis, beam_angle);
    isotope->Decay(photon_number, time, source_num, position);
    return(position);
}

bool BeamPointSource::Inside(const VectorR3 & pos) const
{
    return false;
}
