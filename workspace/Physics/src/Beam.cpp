#include <Physics/Beam.h>

using namespace std;

Beam::Beam()
{
    Reset();
}

void Beam::SetBeam(const VectorR3 & axis, double angle)
{
    beam_axis = axis;
    beam_angle = angle;
}

void Beam::Decay(int photon_number, double time, int src_id,
                 const VectorR3 & position)
{
    beam.SetBeam(beam_axis, beam_angle);
    beam.Decay(photon_number, time, src_id, position);
}

void Beam::Reset()
{
    beam.Reset();
    Isotope::Reset();
}
