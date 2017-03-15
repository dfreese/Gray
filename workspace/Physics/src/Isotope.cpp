#include <Physics/Isotope.h>

Isotope::Isotope()
{
}

Photon Isotope::NextPhoton()
{
    Photon val = daughter.top();
    daughter.pop();
    return val;
}

bool Isotope::IsEmpty() const
{
    return(daughter.empty());
}

void Isotope::AddPhoton(Photon &p)
{
    daughter.push(p);
}

std::ostream& operator<< (std::ostream & os, const Isotope & i)
{
    return i.print_on(os);
}
