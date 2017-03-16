#include <Sources/EllipsoidSource.h>
#include <Random/Random.h>

EllipsoidSource::EllipsoidSource() :
    Source(),
    radius1(1.0),
    radius2(1.0),
    radius3(1.0),
    axis1(1.0, 0.0, 0.0),
    axis2(0.0, 1.0, 0.0),
    axis3(0.0, 0.0, 1.0)
{
    position.SetZero();
}

EllipsoidSource::EllipsoidSource(const VectorR3 &center, const VectorR3 &a1,
                                 const VectorR3 &a2, double r1, double r2,
                                 double r3, double act) :
    Source(center, act),
    radius1(r1),
    radius2(r2),
    radius3(r3),
    axis1(a2),
    axis2(a1 * a2),
    axis3(a1)
{
    //FIXME: Ellipsoids DO NOT ROTATE
    RotMtrx.Set(1, 0, 0,
                0, 1, 0,
                0, 0, 1);
    RotMtrxInv = RotMtrx;
    RotMtrxInv.MakeTranspose();

}

VectorR3 EllipsoidSource::Decay(int photon_number, double time)
{

    //FIXME: Sources are not rotating -- FIXED 01-13-2020 AVDB
    //FIXME: Inside is not rotating -- BUG PDO

    if (isotope == NULL) {
        return(VectorR3(0,0,0));
    }

    VectorR3 p;
    double r1sq = radius1*radius1;
    double r2sq = radius2*radius2;
    double r3sq = radius3*radius3;

    // ellipsoid test
    do {
        p.x = (1.0 - 2.0*Random::Uniform())*radius1;
        p.y = (1.0 - 2.0*Random::Uniform())*radius2;
        p.z = (1.0 - 2.0*Random::Uniform())*radius3;
    } while ( (p.x*p.x/r1sq + p.y*p.y/r2sq + p.z*p.z/r3sq) > 1 );

    VectorR3 roted;
    roted = RotMtrx*p;
    roted += position;
    isotope->Decay(photon_number, time, source_num, roted);
    return(roted);
}

void EllipsoidSource::SetRadius(double r1, double r2, double r3)
{
    radius1 = r1;
    radius2 = r2;
    radius3 = r3;
}

void EllipsoidSource::SetAxis(const VectorR3 &a1, const VectorR3 &a2)
{
    axis1 = a2;
    axis2 = a1*a2;
    axis3 = a1;
}

bool EllipsoidSource::Inside(const VectorR3 & pos) const
{

    //FIXME: Ellipsoids DO NOT ROTATE

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
    c.z = roted.z;

    double r1sq = radius1*radius1;
    double r2sq = radius2*radius2;
    double r3sq = radius3*radius3;

    // ellipsoid test
    if ( (c.x*c.x/r1sq + c.y*c.y/r2sq + c.z*c.z/r3sq) < 1 ) {
        return true;
    } else {
        return false;
    }
}
