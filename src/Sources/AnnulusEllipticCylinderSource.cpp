/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Sources/AnnulusEllipticCylinderSource.h"
#include <cmath>
#include <algorithm>
#include "Gray/Random/Random.h"

using namespace std;

AnnulusEllipticCylinderSource::AnnulusEllipticCylinderSource(
        const VectorR3& position,
        double radius1, double radius2, double height,
        const VectorR3& axis, double activity) :
    Source(position, activity),
    radius1(radius1),
    radius2(radius2),
    height(height),
    axis(axis),
    local_to_global(RefAxisPlusTransToMap(axis, position)),
    global_to_local(local_to_global.Inverse()),
    circ(GenerateTable(radius1, radius2))
{
}

double AnnulusEllipticCylinderSource::EllipticK(double m1) {
    // Appears to be a part of funcitons to calculate the integral along or of
    // an ellipse from the Cephes Math Library.
    //
    // https://boutell.com/lsm/lsmbyid.cgi/000626
    m1 = 1.0 - m1;

    if( m1< DBL_MIN ) {
        return (1.3862943611198906188E0-0.5*log(m1));
    }
    double p = 1.37982864606273237150E-4;
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
    double q = 2.94078955048598507511E-5;
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
    return (p-q*log(m1));
}


double AnnulusEllipticCylinderSource::EllipticE(double m) {
    // Appears to be a part of funcitons to calculate the integral along or of
    // an ellipse from the Cephes Math Library.
    //
    // https://boutell.com/lsm/lsmbyid.cgi/000626
    m = 1.0 - m;
    if (m == 0) {
        return (1.0);
    }

    double p = 1.53552577301013293365E-4;
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
    double q = 3.27954898576485872656E-5;
    q = q*m+1.00962792679356715133E-3;
    q = q*m+6.50609489976927491433E-3;
    q = q*m+1.68862163993311317300E-2;
    q = q*m+2.61769742454493659583E-2;
    q = q*m+3.34833904888224918614E-2;
    q = q*m+4.27180926518931511717E-2;
    q = q*m+5.85936634471101055642E-2;
    q = q*m+9.37499997197644278445E-2;
    q = q*m+2.49999999999888314361E-1;
    return (p-q*m*log(m));
}

double AnnulusEllipticCylinderSource::InverseEllipticE(double arc_length) const
{
    // Appears to be a part of funcitons to calculate the integral along or of
    // an ellipse from the Cephes Math Library.
    //
    // https://boutell.com/lsm/lsmbyid.cgi/000626
    //
    // The line integral along the ellipse is stored in the circ table, and then
    // use the CDF inversion method to select the angle.
    vector<double>::const_iterator val_iter = std::lower_bound(circ.cbegin(),
                                                               circ.cend(),
                                                               arc_length);
    size_t idx = std::distance(circ.cbegin(), val_iter);
    return 2.0 * M_PI * ((double) idx) / ((double) circ.size());
}

double AnnulusEllipticCylinderSource::IncompleteEllipticE(double phi, double m)
{
    // Appears to be a part of funcitons to calculate the integral along or of
    // an ellipse from the Cephes Math Library.
    //
    // https://boutell.com/lsm/lsmbyid.cgi/000626

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
    if (a == 0) {
        temp = sin(lphi);
        if (s < 0) {
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
    if(fabs(t) > 10) {

        /*
         * Transform the amplitude
         */
        e = 1.0/(b*t);

        /*
         * ... but avoid multiple recursions.
         */
        if (fabs(e) < 10) {
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

VectorR3 AnnulusEllipticCylinderSource::Decay() const {
    double C = circ[circ.size()-1];
    double C_uniform = C * Random::Uniform();
    double phi = InverseEllipticE(C_uniform);
    double bcp = radius2*cos(phi);
    double asp = radius1*sin(phi);
    double radius = radius1*radius2/(sqrt(bcp*bcp + asp*asp));

    VectorR3 positron;
    positron.x = radius*cos(phi);
    positron.y = radius*sin(phi);
    positron.z = height * (0.5 - Random::Uniform());
    return(local_to_global * positron);
}

bool AnnulusEllipticCylinderSource::Inside(const VectorR3&) const {
    // Annulus has no width
    return false;
}

std::vector<double> AnnulusEllipticCylinderSource::GenerateTable(
        double radius1, double radius2)
{
    constexpr size_t table_size = 100000;
    constexpr double step = (2.0 * M_PI) / ((double) table_size);
    double m = 1-(radius2/radius1)*(radius2/radius1);
    std::vector<double> circ(table_size);
    circ.clear();
    for (size_t i = 0; i < table_size; i++) {
        circ.push_back(radius1 * IncompleteEllipticE(((double)i)*step, m));
    }
    return (circ);
}
