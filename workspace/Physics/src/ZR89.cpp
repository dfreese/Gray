#include <Physics/ZR89.h>
#include <Random/Random.h>

using namespace std;

ZR89::ZR89(double acolinearity_deg_fwhm) :
    Positron(acolinearity_deg_fwhm)
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

void ZR89::Decay(int photon_number, double time, int src_id,
                 const VectorR3 & position)
{
    p.Reset();
    g.Reset();

    VectorR3 pos = position;
    PositronDecay::PositronRange(pos, positronC, positronK1, positronK2,
                                 positronMaxRange);
    // Calculate Physics to determine when and if Positron and Gamma are
    // emitted together
    if (Random::Uniform() < CONST_PROB_ZR89_POS) {
        AddNuclearDecay(&p);
        p.Decay(photon_number, time, src_id, pos);
    }

    // Gamma is emitted for every positron
    AddNuclearDecay(&g);
    g.Decay(photon_number, time, src_id, pos, CONST_E_ZR89_GAMMA);
}

void ZR89::Reset()
{
    p.Reset();
    g.Reset();
    while (!daughter.empty()) {
        daughter.pop();
    }
}
