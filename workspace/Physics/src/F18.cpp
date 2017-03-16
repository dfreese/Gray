#include <Physics/F18.h>

using namespace std;

// TODO: Generate energy from beta distribution
// TODO: Get true max endpoint in MeV
// const double F18_MAX_BETA_ENERGY=0.315;

F18::F18()
{
    /*******************************************************************************
     *            18F             11C            13N               15O             *
     *  C    0.519 (0.516)   0.501 (0.488)    0.433 (0.426)     0.263 (0.379)      *
     *  k1   27.9 (37.9)     24.5 (23.8)      25.4 (20.2)       33.2 (18.1)        *
     *  k2   2.91 (3.1)      1.76 (1.8)       1.44 (1.4)        1.0 (0.9)          *
     *******************************************************************************/
    positronC = 0.519;
    positronK1 = 27.9;
    positronK2 = 2.91;
    positronMaxRange = 3.0;
    Reset();
}

void F18::Decay(int photon_number, double time, int src_id,
                const VectorR3 & position)
{
    p.Reset();
    AddNuclearDecay(&p);
    p.Decay(photon_number, time, src_id, position, positronC, positronK1,
            positronK2, positronMaxRange);
}

void F18::Reset()
{
    p.Reset();
    while (!daughter.empty()) {
        daughter.pop();
    }
}
