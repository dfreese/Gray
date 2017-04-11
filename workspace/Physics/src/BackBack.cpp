#include <Physics/BackBack.h>
#include <limits>

using namespace std;

BackBack::BackBack(double acolinearity_deg_fwhm) :
    Positron(acolinearity_deg_fwhm, std::numeric_limits<double>::infinity())
{
    Reset();
}

void BackBack::Decay(int photon_number, double time, int src_id,
                     const VectorR3 & position)
{
    p.Reset();
    AddNuclearDecay(&p);
    p.Decay(photon_number, time, src_id, position);
}
