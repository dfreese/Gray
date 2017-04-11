#include <Physics/ZR89.h>

using namespace std;

ZR89::ZR89(double acolinearity_deg_fwhm) :
    GammaPositron(acolinearity_deg_fwhm,
                  282280.32, // Half-life in seconds
                  0.90915, // Gamma Energy in MeV
                  0.227) // 22.7% of decays lead to a positron
{
    positronC = 0.519;
    positronK1 = 27.9;
    positronK2 = 2.91;
    positronMaxRange = 3.0;
    Reset();
}

void ZR89::Decay(int photon_number, double time, int src_id,
                 const VectorR3 & position)
{
    AddNuclearDecay(&gp);
    gp.Decay(photon_number, time, src_id, position, positronC, positronK1,
             positronK2, positronMaxRange);
}
