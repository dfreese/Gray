#include <Sources/CylinderSource.h>
#include <Random/Random.h>

CylinderSource::CylinderSource() :
    Source(),
    radius(1.0),
    length(1.0),
    axis(0.0, 0.0, 0.0)
{
}

CylinderSource::CylinderSource(const VectorR3 &p, double rad, VectorR3 L, double act) :
    Source(p, act)
{
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

VectorR3 CylinderSource::Decay(int photon_number, double time)
{
    VectorR3 positron = RotMtrx * Random::UniformCylinder(length, radius) + position;
    isotope->Decay(photon_number, time, source_num, positron);
    return(positron);
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
    // TODO: refactor this out for all cylinders.
    const VectorR3 roted = RotMtrxInv * (pos - position);
    if ((roted.x * roted.x + roted.y * roted.y) > radius * radius) {
        return false;
    }
    if (std::abs(roted.z) > length/2.0) {
        return false;
    }
    return true;
}
