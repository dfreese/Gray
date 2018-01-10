#include <Physics/Isotope.h>
#include <cmath>

Isotope::Isotope(double half_life_s) :
    half_life(half_life_s)
{
}

double Isotope::GetHalfLife() const {
    return(half_life);
}

double Isotope::FractionRemaining(double time) const {
    // Note: this will produce a nan at time=infinity for infinite half_life.
    // Please don't do that....
    return(std::pow(2.0, -time / half_life));
}

