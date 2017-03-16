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
    p.Reset();
    p.source_num = src_id;
    p.SetTime(time);
    p.SetPosition(position);
    p.SetBeam(beam_axis, beam_angle);
    p.Decay(photon_number);
}

void Beam::Reset()
{
    p.Reset();
    while (!daughter.empty()) {
        daughter.pop();
    }
}
