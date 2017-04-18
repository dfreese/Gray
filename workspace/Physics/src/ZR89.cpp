#include <Physics/ZR89.h>

using namespace std;

ZR89::ZR89(double acolinearity_deg_fwhm) :
    Positron(acolinearity_deg_fwhm,
             282280.32, // Half-life in seconds
             0.227, // 22.7% of decays lead to a positron
             0.90915) // Gamma Energy in MeV
{
    SetPositronRange(0.519, 27.9, 2.91, 3.0);
    Reset();
}
