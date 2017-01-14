#include "AnnulusEllipticCylinderSource.h"

AnnulusEllipticCylinderSource::AnnulusEllipticCylinderSource()
{
    position.SetZero();
    radius1 = 1.0;
    radius2 = 1.0;
    axis = UnitVecKR3;
    length = 1.0;
}

AnnulusEllipticCylinderSource::AnnulusEllipticCylinderSource(const VectorR3 &p, double r1, double r2, VectorR3 &L, double act)
{
    unsigned int i;
    position = p;
    SetActivity(act);
    radius1 = r1;
    radius2 = r2;
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

    double step = (2.0 * M_PI)/((double)NUM_TABLE);
    double m = 1-(radius2/radius1)*(radius2/radius1);
    cout << "Test elliptic integrals:m[" << m << "]\n";
    for (i = 0; i < NUM_TABLE; i++) {
        //cout << "i:[" << i << "][";
        circ.push_back(radius1 * IncompleteEllipticE(((double)i)*step, m));
        //cout << circ[i] << "\n";
    }
}

double AnnulusEllipticCylinderSource::EllipticK(double m1)
{
    double p;
    double q;
    double result;
    m1 = 1.0 - m1;

    if( m1< DBL_MIN ) {
        result = 1.3862943611198906188E0-0.5*log(m1);
    } else {
        p = 1.37982864606273237150E-4;
        p = p*m1+2.28025724005875567385E-3;
        p = p*m1+7.97404013220415179367E-3;
        p = p*m1+9.85821379021226008714E-3;
        p = p*m1+6.87489687449949877925E-3;
        p = p*m1+6.18901033637687613229E-3;
        p = p*m1+8.79078273952743772254E-3;
        p = p*m1+1.49380448916805252718E-2;
        p = p*m1+3.08851465246711995998E-2;
        p = p*m1+9.65735902811690126535E-2;
        p = p*m1+1.38629436111989062502E0;
        q = 2.94078955048598507511E-5;
        q = q*m1+9.14184723865917226571E-4;
        q = q*m1+5.94058303753167793257E-3;
        q = q*m1+1.54850516649762399335E-2;
        q = q*m1+2.39089602715924892727E-2;
        q = q*m1+3.01204715227604046988E-2;
        q = q*m1+3.73774314173823228969E-2;
        q = q*m1+4.88280347570998239232E-2;
        q = q*m1+7.03124996963957469739E-2;
        q = q*m1+1.24999999999870820058E-1;
        q = q*m1+4.99999999999999999821E-1;
        result = p-q*log(m1);
    }
    return result;
}


double AnnulusEllipticCylinderSource::EllipticE(double m)
{
    double p;
    double q;
    double result;


    m = 1.0 - m;

    if( m == 0) {
        result = 1;
        return result;
    }

    p = 1.53552577301013293365E-4;
    p = p*m+2.50888492163602060990E-3;
    p = p*m+8.68786816565889628429E-3;
    p = p*m+1.07350949056076193403E-2;
    p = p*m+7.77395492516787092951E-3;
    p = p*m+7.58395289413514708519E-3;
    p = p*m+1.15688436810574127319E-2;
    p = p*m+2.18317996015557253103E-2;
    p = p*m+5.68051945617860553470E-2;
    p = p*m+4.43147180560990850618E-1;
    p = p*m+1.00000000000000000299E0;
    q = 3.27954898576485872656E-5;
    q = q*m+1.00962792679356715133E-3;
    q = q*m+6.50609489976927491433E-3;
    q = q*m+1.68862163993311317300E-2;
    q = q*m+2.61769742454493659583E-2;
    q = q*m+3.34833904888224918614E-2;
    q = q*m+4.27180926518931511717E-2;
    q = q*m+5.85936634471101055642E-2;
    q = q*m+9.37499997197644278445E-2;
    q = q*m+2.49999999999888314361E-1;
    result = p-q*m*log(m);
    return result;
}

#define EQUAL_PRECISION 1e-2
bool double_compare(double i, double j)
{
    if (fabs(i - j) < EQUAL_PRECISION) {
        return false;
    } else {
        return (i<j);
    }
}

int binarySearch(const vector<double> & array, int first, int last, double key)
{
    // function:
    //   Searches sortedArray[first]..sortedArray[last] for key.
    // returns: index of the matching element if it finds key,
    //         otherwise  -(index where it could be inserted)-1.
    // parameters:
    //   sortedArray in  array of sorted (ascending) values.
    //   first, last in  lower and upper subscript bounds
    //   key         in  value to search for.
    // returns:
    //   index of key, or -insertion_position -1 if key is not
    //                 in the array. This value can easily be
    //                 transformed into the position to insert it.

    while (first <= last) {
        int mid = (first + last) / 2;  // compute mid point.
        if (double_compare(array[mid],key)) {
            first = mid + 1;    // repeat search in top half.
        } else if (double_compare(key,array[mid])) {
            last = mid - 1;    // repeat search in bottom half.
        } else {
            return mid;    // found it. return position /////
        }
    }
    return -(first + 1);    // failed to find key
}

double AnnulusEllipticCylinderSource::InverseEllipticE(double arc_length)
{
    int index;
    index = binarySearch(circ, 0, circ.size()-1, arc_length);
    if (index < 0) {
        cout << "Failed to find arc_length\n";
        return -1.0;
    } else {
        return 2.0 * M_PI * ((double)index)/((double)NUM_TABLE);
    }
}

double AnnulusEllipticCylinderSource::IncompleteEllipticE(double phi, double m)
{

    double pio2;
    double a;
    double b;
    double c;
    double e;
    double temp;
    double lphi;
    double t;
    double ebig;
    long d;
    long md;
    long npio2;
    long s;
    double result;


    pio2 = 1.57079632679489661923;

    if( m == 0) {
        result = phi;
        return result;
    }

    lphi = phi;
    npio2 = floor(lphi/pio2);
    if( npio2%2!=0 ) {
        npio2 = npio2+1;
    }
    lphi = lphi-npio2*pio2;
    if( lphi<0 ) {
        lphi = -lphi;
        s = -1;
    } else {
        s = 1;
    }
    a = 1.0-m;
    ebig = EllipticE(m);
    if( (a==0) ) {
        temp = sin(lphi);
        if( s<0 ) {
            temp = -temp;
        }
        result = temp+npio2*ebig;
        return result;
    }
    t = tan(lphi);
    b = sqrt(a);

    /*
     * Thanks to Brian Fitzgerald <fitzgb@mml0.meche.rpi.edu>
     * for pointing out an instability near odd multiples of pi/2
     */
    if( fabs(t>10) ) {

        /*
         * Transform the amplitude
         */
        e = 1.0/(b*t);

        /*
         * ... but avoid multiple recursions.
         */
        if( fabs(e)<10) {
            e = atan(e);
            temp = ebig+m*sin(lphi)*sin(e)-IncompleteEllipticE(e, m);
            if( s<0 ) {
                temp = -temp;
            }
            result = temp+npio2*ebig;
            return result;
        }
    }
    c = sqrt(m);
    a = 1.0;
    d = 1;
    e = 0.0;
    md = 0;
    // Find the right precision
    while(fabs(c/a)>1e-15) {
        temp = b/a;
        lphi = lphi+atan(t*temp)+md*M_PI;
        md = trunc((lphi+pio2)/M_PI);
        t = t*(1.0+temp)/(1.0-temp*t*t);
        c = 0.5*(a-b);
        temp = sqrt(a*b);
        a = 0.5*(a+b);
        b = temp;
        d = d+d;
        e = e+c*sin(lphi);
    }
    temp = ebig/EllipticK(m);
    temp = temp*((atan(t)+md*M_PI)/(d*a));
    temp = temp+e;
    if( s<0 ) {
        temp = -temp;
    }
    result = temp+npio2*ebig;
    return result;
}


void AnnulusEllipticCylinderSource::Decay(unsigned int photon_number)
{

    //FIXME: Sources are not rotating -- FIXED 01-13-2020 AVDB
    //FIXME: Inside is not rotating -- BUG PDO

    double C = circ[circ.size()-1];
    double C_uniform;
    double phi;
    double radius;

    if (isotope == NULL) {
        return;
    }
    VectorR3 positron;

    // Generate a uniform from 0 to C
    do {
        C_uniform = C*Random();
    } while (C_uniform < 0.0);

    // Calculate the phi angle
    phi = InverseEllipticE(C_uniform);
    //cout << "Circ:[" << C << "]Arclength:[" << C_uniform << "]Phi:[";
    //

    double bcp = radius2*cos(phi);
    double asp = radius1*sin(phi);

    radius = radius1*radius2/(sqrt(bcp*bcp + asp*asp));
    //cout << phi << "]radius:[" << radius << "]\n";

    positron.x = radius*cos(phi);
    positron.y = radius*sin(phi);
    positron.z = length * (0.5 - Random());

    VectorR3 roted;
    roted = RotMtrx*positron;
    roted += position;
    isotope->SetPosition(roted);
    isotope->SetMaterial(GetMaterial());
    isotope->Decay(photon_number);
}

void AnnulusEllipticCylinderSource::SetRadius(double r1, double r2)
{
    radius1 = r1;
    radius2 = r2;
}

void AnnulusEllipticCylinderSource::SetAxis(VectorR3 &L)
{
    length = L.Norm();
    axis = L.MakeUnit();
}

bool AnnulusEllipticCylinderSource::Inside(const VectorR3 & pos) const
{

    if (isotope == NULL) {
        return false;
    }

    // Annulus has not width
    return false;

}
