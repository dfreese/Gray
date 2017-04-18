#include <Physics/IN110.h>

using namespace std;

IN110::IN110(double acolinearity_deg_fwhm) :
    GammaPositron(acolinearity_deg_fwhm,
                  17676.0, // Half-life in seconds
                  0.61, // 61% of decays lead to a positron
                  0.657750) // Gamma Decays are 0.657750 MeV
{
    positronC = 0.519;
    positronK1 = 27.9;
    positronK2 = 2.91;
    positronMaxRange = 3.0;
    Reset();
}

void IN110::Decay(int photon_number, double time, int src_id,
                  const VectorR3 & position)
{
    AddNuclearDecay(&gp);
    gp.Decay(photon_number, time, src_id, position, positronC, positronK1,
             positronK2, positronMaxRange);
}
