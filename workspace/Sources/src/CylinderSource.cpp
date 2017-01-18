#include <Sources/CylinderSource.h>
#include <Random/Random.h>

CylinderSource::CylinderSource()
{
    position.SetZero();
    radius = 1.0;
    axis = UnitVecKR3;
    length = 1.0;
}

CylinderSource::CylinderSource(const VectorR3 &p, double rad, VectorR3 L, double act)
{
    position = p;
    SetActivity(act);
    radius = rad;
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

void CylinderSource::Decay(unsigned int photon_number)
{
    if (isotope == NULL) {
        return;
    }

    VectorR3 positron;
    do {
        positron.x = (1.0 - 2.0*Random::Uniform())*radius;
        positron.y = (1.0 - 2.0*Random::Uniform())*radius;
        positron.z = 0;
    } while (positron.Norm() > radius);
    positron.z = length*(0.5 - Random::Uniform());

    VectorR3 roted;
    roted = RotMtrx*positron;
    roted += position;
    isotope->SetPosition(roted);
    isotope->Decay(photon_number);
}

void CylinderSource::SetRadius(double r)
{
    radius = r;
}

void CylinderSource::SetAxis(VectorR3 L)
{
    length = L.Norm();
    axis = L.MakeUnit();
}

bool CylinderSource::Inside(const VectorR3 & pos) const
{

    if (isotope == NULL) {
        return false;
    }

    VectorR3 dist;
    dist = pos;
    dist -= position;
    VectorR3 roted;

    roted = RotMtrxInv*dist;

    VectorR3 c;
    c.x = roted.x;
    c.y = roted.y;
    c.z = 0.0;

    if (c.Norm() > radius) {
        return false;
    }
    if (fabs(roted.z) > length/2.0) {
        return false;
    }
    return true;
}
