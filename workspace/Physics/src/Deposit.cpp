#include <Physics/Deposit.h>
#include <stdio.h>

Deposit::Deposit()
{
    Reset();
}
Deposit::~Deposit() {}

void Deposit::Reset()
{
    energy = -1.0;
    time = 0.0;
    pos.SetZero();
    id = -1;
    det_id = -1;
    src_id = -1;
    det_id = -1;
    mat_id = -1;
    color = Photon::P_BLUE;
    type = I_EMPTY;
}

ostream& operator<< ( ostream& os, const Deposit& d )
{
    char str[256];

    switch (d.type) {
    case I_POSITRON:
        os << "0 ";
        break;
    case I_COMPTON:
        os << "1 ";
        break;
    case I_RAYLEIGH:
        os << "2 ";
        break;
    case I_PHOTOELECTRIC:
        os << "3 ";
        break;
    case I_GAMMA_DECAY:
        os << "4 ";
        break;
    case I_BETA_DECAY:
        os << "5 ";
        break;
    case I_AUGER:
        os << "6 ";
        break;
    case I_XRAY_ESCAPE:
        os << "7 ";
        break;
    case I_EMPTY:
    case I_DEBUG:
    case I_ERROR:
    default:
        os << "-1 ";
    }
    os << d.id;
    os << " " << d.color << " ";
    sprintf(str,"%23.16e ",d.time);
    os << str;
    sprintf(str,"%12.6e ",d.energy);
    os << str;
    sprintf(str,"%15.8e %15.8e %15.8e %2d ",(float) d.pos.x, (float) d.pos.y, (float) d.pos.z, d.src_id);
    os << str;
    return os;
}

bool Deposit::operator<(const Deposit &d2)
{
    return !(time <= d2.time);
}
