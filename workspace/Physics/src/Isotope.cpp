#include <Physics/Isotope.h>

Isotope::Isotope()
{
}

void Isotope::SetTime(const double t)
{
    time = t;
}

void Isotope::SetPosition(const VectorR3 & pos)
{
    position = pos;
}

void Isotope::SetId(long i)
{
    id = i;
}

long Isotope::GetId() const
{
    return id;
}

const VectorR3 & Isotope::GetPosition() const
{
    return position;
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
