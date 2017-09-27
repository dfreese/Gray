#include <Sources/AnnulusCylinderSource.h>
#include <Random/Random.h>

AnnulusCylinderSource::AnnulusCylinderSource() :
    Source(),
    radius(1.0),
    length(1.0),
    axis(0.0, 0.0, 1.0)
{
}

AnnulusCylinderSource::AnnulusCylinderSource(const VectorR3 &p, double rad, VectorR3 L, double act) :
    Source(p, act)
{
    radius = rad;
    length = L.Norm();
    axis = L.MakeUnit();
    // calculate Rotation Matrix

    //cout << "L.x = " << L.x << "  L.y = " << L.y << "  L.z = " << L.z <<endl;
    //cout << "axis.x = " << axis.x << "  axis.y = " << axis.y << "  axis.z = " << axis.z <<endl;
    /* Rotation Matrix based on Logbook 4 p72, AVDB) */
    double c= axis.z;
    double s=(axis.x*axis.x+axis.y*axis.y);
    RotMtrx.Set( axis.y*axis.y + (1-axis.y*axis.y)*c, -axis.x*axis.y*(1-c), -axis.x*s,
                 -axis.x*axis.y*(1-c), axis.x*axis.x + ( 1-axis.x*axis.x)*c, axis.y*s,
                 axis.x*s, axis.y*s,c);
    RotMtrxInv = RotMtrx;
    RotMtrxInv.MakeTranspose();
}

VectorR3 AnnulusCylinderSource::Decay(int photon_number, double time)
{
    VectorR3 roted = RotMtrx * Random::UniformAnnulusCylinder(length, radius);
    roted += position;
    isotope->Decay(photon_number, time, source_num, roted);
    return(roted);
}

void AnnulusCylinderSource::SetRadius(double r)
{
    radius = r;
}

void AnnulusCylinderSource::SetAxis(VectorR3 L)
{
    length = L.Norm();
    axis = L.MakeUnit();
}

bool AnnulusCylinderSource::Inside(const VectorR3 & pos) const
{
    // FIXME: This calculates if something is inside of the cylinder, but
    // doesn't calculate if it was inside the annulus.  This would make it
    // incorrect for a negative source (which wouldn't really make sense for
    // an annulus).
    const VectorR3 roted = RotMtrxInv * (pos - position);
    if ((roted.x * roted.x + roted.y * roted.y) > radius * radius) {
        return false;
    }
    if (std::abs(roted.z) > length/2.0) {
        return false;
    }
    return true;
}
