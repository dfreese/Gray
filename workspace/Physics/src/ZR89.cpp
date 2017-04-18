#include <Physics/ZR89.h>

using namespace std;

ZR89::ZR89(double acolinearity_deg_fwhm) :
    GammaPositron(acolinearity_deg_fwhm,
                  282280.32, // Half-life in seconds
                  0.227, // 22.7% of decays lead to a positron
                  0.90915) // Gamma Energy in MeV
{
    positronC = 0.519;
    positronK1 = 27.9;
    positronK2 = 2.91;
    positronMaxRange = 3.0;
    use_positron_dbexp = true;
    use_positron_gauss = false;
    Reset();
}
