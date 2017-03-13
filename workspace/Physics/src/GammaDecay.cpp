#include <Physics/GammaDecay.h>
#include <stdio.h>
#include <math.h>
#include <Random/Random.h>

GammaDecay::GammaDecay()
{
    gamma.color = Photon::P_YELLOW;
}

void GammaDecay::Decay(unsigned int photon_number)
{

    gamma.Reset();
    gamma.time = time;
    gamma.pos = pos;
    gamma.energy = energy;
    gamma.id = photon_number;
    gamma.det_id = -1;
    gamma.color = Photon::P_YELLOW;
    gamma.dir.SetUnitZ();
    gamma.src_id = source_num;
    Random::UniformSphere(gamma.dir);
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
