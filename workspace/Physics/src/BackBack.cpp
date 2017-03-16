#include <Physics/BackBack.h>

using namespace std;

BackBack::BackBack()
{
    Reset();
}

void BackBack::Decay(int photon_number, double time, int src_id,
                     const VectorR3 & position)
{
    p.Reset();
    p.source_num = src_id;
    p.SetTime(time);
    p.SetPosition(position);
    AddNuclearDecay(&p);
    p.Decay(photon_number);
}

void BackBack::Reset()
{
    p.Reset();
    while (!daughter.empty()) {
        daughter.pop();
    }
}

ostream & BackBack::print_on(ostream & os) const
{
    os << "BackBack: ";
    os << p;
    return os;
}
