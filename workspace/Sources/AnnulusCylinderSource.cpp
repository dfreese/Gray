#include <AnnulusCylinderSource.h>

AnnulusCylinderSource::AnnulusCylinderSource()
{
    position.SetZero();
    radius = 1.0;
    axis = UnitVecKR3;
    length = 1.0;
}

AnnulusCylinderSource::AnnulusCylinderSource(const VectorR3 &p, double rad, VectorR3 L, double act)
{
    position = p;
    SetActivity(act);
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

void AnnulusCylinderSource::Decay(unsigned int photon_number)
{

    //FIXME: Sources are not rotating -- FIXED 01-13-2020 AVDB
    //FIXME: Inside is not rotating -- BUG PDO

    if (isotope == NULL) {
        return;
    }

    VectorR3 positron;
    double phi = M_2_PI * Random();
    positron.x = radius*cos(phi);
    positron.y = radius*sin(phi);
    positron.z = length*(0.5 - Random());

    VectorR3 roted;
    roted = RotMtrx*positron;
    roted += position;
    isotope->SetPosition(roted);
    isotope->SetMaterial(GetMaterial());
    isotope->Decay(photon_number);
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
