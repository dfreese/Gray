#include <Physics/Isotope.h>
#include <cmath>
#include <limits>

Isotope::Isotope() :
    half_life(std::numeric_limits<double>::infinity())
{
}

Isotope::Isotope(double half_life_s) :
    half_life(half_life_s)
{
}

NuclearDecay * Isotope::NextNuclearDecay()
{
    NuclearDecay * val = daughter.top();
    daughter.pop();
    return val;
}

bool Isotope::IsEmpty() const
{
    return(daughter.empty());
}

void Isotope::AddNuclearDecay(NuclearDecay * nd)
{
    daughter.push(nd);
}

void Isotope::Reset() {
    while (!daughter.empty()) {
        daughter.pop();
    }
}

double Isotope::FractionRemaining(double time) const {
    return(std::pow(0.5, time / half_life));
}
