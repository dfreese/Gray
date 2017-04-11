#include <Physics/O15.h>

using namespace std;

// TODO: Generate energy from beta distribution
// TODO: Get true max endpoint in MeV
// const double F18_MAX_BETA_ENERGY=0.315;

O15::O15(double acolinearity_deg_fwhm) :
    Positron(acolinearity_deg_fwhm,
             122.46, // Half-life in seconds
             0.99885) // Positron Emission Probability
{
    positronC = 0.263;
    positronK1 = 33.2;
    positronK2 = 1.0;
    positronMaxRange = 3.0;
    Reset();
}

void O15::Decay(int photon_number, double time, int src_id,
                const VectorR3 & position)
{
    p.Reset();
    AddNuclearDecay(&p);
    p.Decay(photon_number, time, src_id, position, positronC, positronK1,
            positronK2, positronMaxRange);
}
