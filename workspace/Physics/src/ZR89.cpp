#include <Physics/ZR89.h>
#include <Random/Random.h>

using namespace std;

ZR89::ZR89()
{
    /*******************************************************************************
     *            18F             11C            13N               15O             *
     *  C    0.519 (0.516)   0.501 (0.488)    0.433 (0.426)     0.263 (0.379)      *
     *  k1   27.9 (37.9)     24.5 (23.8)      25.4 (20.2)       33.2 (18.1)        *
     *  k2   2.91 (3.1)      1.76 (1.8)       1.44 (1.4)        1.0 (0.9)          *
     *******************************************************************************/
    positronRange = true;
    positronRangeGaussian = false;
    positronRangeCusp = true;
    positronFWHM = 1.0; // expressed in meters
    positronC = 0.519;
    positronK1 = 27.9;
    positronK2 = 2.91;
    positronMaxRange = 3.0;
    g.SetEnergy(CONST_E_ZR89_GAMMA);
    Reset();
}

void ZR89::Decay(unsigned int photon_number, double time,
                 const VectorR3 & position)
{
    // TODO: Find ZR89 Beta energy
    SetEnergy(0.315);
    p.source_num = source_num;
    g.source_num = source_num;
    p.SetTime(time);
    g.SetTime(time);
    p.SetPosition(position);
    g.SetPosition(position);
    // Get Rid of Redundant Positron Range code in Isotopes

    PositronRange(p);
    p.Decay(photon_number);

    // No Gamma Decay for Gamma Rays
    g.Decay(photon_number);

    // Calculate Physics to determine when and if Positron and Gamma are emitted together

    if (Random::Uniform() < CONST_PROB_ZR89_POS) {
        AddPhoton(p.blue);
        AddPhoton(p.red);
    }
    // Gamma is emitted for every positron
    AddPhoton(g.gamma);
}

void ZR89::Reset()
{
    p.Reset();
    g.Reset();
    while (!daughter.empty()) {
        daughter.pop();
    }
}

ostream & ZR89::print_on(ostream & os) const
{
    os << "ZR89: ";
    os << p;
    return os;
}
