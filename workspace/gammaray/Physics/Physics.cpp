#include <Physics/Physics.h>
#include <cmath>
#include <Math/Math.h>
#include <Physics/GammaStats.h>
#include <Physics/NuclearDecay.h>
#include <Random/Random.h>
#include <algorithm>

using namespace std;

double Physics::KleinNishinaEnergy(const double energy, const double costheta)
{
    return(energy / (1.0 + (energy / Physics::energy_511) * (1. - costheta)));
}
