#include <Physics/Isotope.h>

Isotope::Isotope()
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
