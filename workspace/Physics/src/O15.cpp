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
    SetPositronRange(0.263, 33.2, 1.0, 3.0);
    Reset();
}
