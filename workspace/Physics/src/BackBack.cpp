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
    AddNuclearDecay(&p);
    p.Decay(photon_number, time, src_id, position);
}

void BackBack::Reset()
{
    p.Reset();
    while (!daughter.empty()) {
        daughter.pop();
    }
}
