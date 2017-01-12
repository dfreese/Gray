#include "GammaDecay.h"

const double CONST_ACOL = (0.47 / 180.0) * PI / 2.35482005;
const double CONST_FWHM_TO_SIGMA = (1.0)/(2.35482005);
const double CONST_MM_TO_CM = (0.1); // 10 mm per cm
GammaDecay::GammaDecay()
{
    gamma.color = P_YELLOW;
}

void GammaDecay::Decay(unsigned int photon_number)
{

    gamma.Reset();
    gamma.time = time;
    gamma.pos = pos;
    gamma.energy = energy;
    gamma.id = photon_number;
    gamma.det_id = -1;
    gamma.color = P_YELLOW;
    gamma.dir.SetUnitZ();
    gamma.src_id = source_num;
    UniformSphere(gamma.dir);
}

void GammaDecay::SetPosition(const VectorR3 & p)
{
    pos = p;
    gamma.pos = p;
}

ostream& GammaDecay::print_on( ostream& os ) const
{
    char str[256];

    os << gamma.id;
    os << " -1 ";
    sprintf(str,"%08.16e ",time);
    os << str;
    sprintf(str,"%04e ",energy);
    os << str;
    // positron is a first interaction
    sprintf(str,"%04e %04e %04e ",pos.x, pos.y, pos.z);
    os << str;

    return os;
}
