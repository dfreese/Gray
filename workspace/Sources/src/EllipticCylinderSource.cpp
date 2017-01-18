#include <Sources/EllipticCylinderSource.h>
#include <Random/Random.h>

EllipticCylinderSource::EllipticCylinderSource()
{
    position.SetZero();
    radius1 = 1.0;
    radius2 = 1.0;
    axis = UnitVecKR3;
    length = 1.0;
}

EllipticCylinderSource::EllipticCylinderSource(const VectorR3 &p, double r1, double r2, VectorR3 &L, double act)
{
    position = p;
    SetActivity(act);
    radius1 = r1;
    radius2 = r2;
    length = L.Norm();
    axis = L.MakeUnit();
    /* Rotation Matrix based on Logbook 4 p72, AVDB) */
    double c= axis.z;
    double s=(axis.x*axis.x+axis.y*axis.y);
    RotMtrx.Set( axis.y*axis.y + (1-axis.y*axis.y)*c, -axis.x*axis.y*(1-c), -axis.x*s,
                 -axis.x*axis.y*(1-c), axis.x*axis.x + ( 1-axis.x*axis.x)*c, axis.y*s,
                 axis.x*s, axis.y*s,c);
    RotMtrxInv = RotMtrx;
    RotMtrxInv.MakeTranspose();
}

void EllipticCylinderSource::Decay(unsigned int photon_number)
{

    //FIXME: Sources are not rotating -- FIXED 01-13-2020 AVDB
    //FIXME: Inside is not rotating -- BUG PDO

    if (isotope == NULL) {
        return;
    }
    double r1sq = radius1*radius1;
    double r2sq = radius2*radius2;

    VectorR3 positron;
    do {
        positron.x = (1.0 - 2.0*Random::Uniform())*radius1;
        positron.y = (1.0 - 2.0*Random::Uniform())*radius2;
        positron.z = 0;
    } while (positron.x*positron.x/r1sq + positron.y*positron.y/r2sq > 1);
    positron.z = length*(0.5 - Random::Uniform());

    VectorR3 roted;
    roted = RotMtrx*positron;
    roted += position;
    isotope->SetPosition(roted);
    isotope->Decay(photon_number);
}

void EllipticCylinderSource::SetRadius(double r1, double r2)
{
    radius1 = r1;
    radius2 = r2;
}

void EllipticCylinderSource::SetAxis(VectorR3 &L)
{
    length = L.Norm();
    axis = L.MakeUnit();
}

bool EllipticCylinderSource::Inside(const VectorR3 & pos) const
{

    if (isotope == NULL) {
        return false;
    }

    double r1sq = radius1*radius1;
    double r2sq = radius2*radius2;

    VectorR3 dist;
    dist = pos;
    dist -= position;
    VectorR3 roted;

    roted = RotMtrxInv*dist;

    VectorR3 c;
    c.x = roted.x;
    c.y = roted.y;
    c.z = 0.0;

    if ( c.x*c.x/r1sq + c.y*c.y/r2sq > 1) {
        return false;
    }
    if (fabs(roted.z) > length/2.0) {
        return false;
    }
    return true;
}
