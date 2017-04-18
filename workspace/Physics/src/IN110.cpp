#include <Physics/IN110.h>

using namespace std;

IN110::IN110(double acolinearity_deg_fwhm) :
    Positron(acolinearity_deg_fwhm,
             17676.0, // Half-life in seconds
             0.61, // 61% of decays lead to a positron
             0.657750) // Gamma Decays are 0.657750 MeV
{
    SetPositronRange(0.519, 27.9, 2.91, 3.0);
    Reset();
}
