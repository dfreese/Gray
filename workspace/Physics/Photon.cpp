#include "Photon.h"
#include <stdio.h>

using namespace std;

Photon::Photon()
{
    Reset();
}

Photon::~Photon()
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

Photon::Photon(int set_id, double e, const VectorR3 & p, const VectorR3 & d)
{

    det_id = -1;
    color = P_BLUE;

    energy = e;
    pos = p;
    dir = d;
    time = 0.0;
    id = set_id;
    phantom_scatter = false;
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

bool operator==(const Photon &p1, const Photon &p2)
{
    if ((p1.id == p2.id) && (p1.color == p2.color)) {
        return true;
    } else {
        return false;
    }
}

Photon& Photon::operator=(const Photon &p2)
{
    pos = p2.pos;
    dir =	p2.dir;
    energy = p2.energy;
    time =	p2.time;
    id =	p2.id;
    color = p2.color;
    det_id =	p2.det_id;
    src_id = p2.src_id;
    phantom_scatter = p2.phantom_scatter;
    return *this;
}

bool Photon::operator<(const Photon &p2)
{
    return !(time <= p2.time);
}
