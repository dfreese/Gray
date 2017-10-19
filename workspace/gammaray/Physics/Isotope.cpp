#include <Physics/Isotope.h>
#include <cmath>
#include <limits>

Isotope::Isotope(double half_life_s) :
    half_life(half_life_s)
{
}

double Isotope::GetHalfLife() const {
    return(half_life);
}

double Isotope::FractionRemaining(double time) const {
    return(std::pow(2.0, -time / half_life));
}

