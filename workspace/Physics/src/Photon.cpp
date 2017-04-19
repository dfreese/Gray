#include <Physics/Photon.h>
#include <stdio.h>

using namespace std;

Photon::Photon()
{
    Reset();
}

void Photon::Reset()
{
    energy = -1.0;
    time = 0.0;
    dir.SetZero();
    pos.SetZero();
    id = 0;
    src_id = -1;
    det_id = -1;
    color = P_BLUE;
    phantom_scatter = false;
}

Photon::Photon(int set_id, double e, const VectorR3 & p, const VectorR3 & d) :
    pos(p),
    dir(d),
    energy(e),
    time(0.0),
    id(set_id),
    color(P_BLUE),
    det_id(-1),
    phantom_scatter(false),
    src_id(-1)
{
}

ostream& operator<< ( ostream& os, const Photon& p )
{
    char str[256];

    os << p.id;
    os << " " << p.color << " ";
    sprintf(str,"%23.16e ",p.time);
    os << str;
    sprintf(str,"%12.6e ",p.energy);
    os << str;
    sprintf(str,"%15.8e %15.8e %15.8e %2d ",(float) p.pos.x, (float) p.pos.y, (float) p.pos.z, p.src_id);
    os << str;
    if (p.phantom_scatter == true ) {
        os << " 1 ";
    } else {
        os << " 0 ";
    }
    return os;
}
