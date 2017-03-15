#include <Physics/BackBack.h>

using namespace std;

BackBack::BackBack()
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
    Reset();
}

void BackBack::Decay(unsigned int photon_number, double time,
                     const VectorR3 & position)
{
    p.source_num = source_num;
    p.SetTime(time);
    p.SetPosition(position);
    //PositronRange(p);
    p.Decay(photon_number);
    AddPhoton(p.blue);
    AddPhoton(p.red);
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
