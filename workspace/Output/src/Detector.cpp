#include <Output/Detector.h>
#include <stdio.h>

Detector::Detector() :
    detector_id(0),
    size(-1.0f, -1.0f, -1.0f),
    pos(-1.0f, -1.0f, -1.0f),
    idx{0, 0, 0},
    block(0)
{
}

Detector::Detector(int id, const VectorR3 & p, const VectorR3 &s,
                   const RigidMapR3 & m)
{
    Init(id,p,s,m,0,0,0,0);
}

Detector::Detector(int id, const VectorR3 & p, const VectorR3 &s,
                   const RigidMapR3 & m, int x, int y, int z, int bl)
{
    Init(id, p, s, m, x, y, z, bl);
}

void Detector::Init(int id, const VectorR3 & p, const VectorR3 &s,
                    const RigidMapR3 & m, int x, int y, int z, int bl)
{
    detector_id = id;
    pos = p;
    size = s;
    map = m;
    idx[0] = x;
    idx[1] = y;
    idx[2] = z;
    block = bl;
}

void Detector::SetBlock(int bl)
{
    block = bl;
}

ostream& operator<< ( ostream& os, const Detector& d )
{
    char str[256];
    os << d.detector_id;
    os << " ";
    sprintf(str,"%08e %08e %08e ",d.size.x, d.size.y, d.size.z);
    os << str;
    sprintf(str,"%08e %08e %08e ",d.pos.x, d.pos.y, d.pos.z);
    os <<  str;
    os << " ";
    os << d.idx[0];
    os << " ";
    os << d.idx[1];
    os << " ";
    os << d.idx[2];
    os << " ";
    os << d.block;
    return os;
}
