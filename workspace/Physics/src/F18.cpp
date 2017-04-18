#include <Physics/F18.h>

using namespace std;

// TODO: Generate energy from beta distribution
// TODO: Get true max endpoint in MeV
// const double F18_MAX_BETA_ENERGY=0.315;

F18::F18(double acolinearity_deg_fwhm) :
    Positron(acolinearity_deg_fwhm,
             6584.04, // Half-life in seconds
             0.9686) // Positron Emission Probability
{
    SetPositronRange(0.519, 27.9, 2.91, 3.0);
    Reset();
}
