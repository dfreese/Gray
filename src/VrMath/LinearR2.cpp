/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/VrMath/LinearR2.h"
#include <assert.h>

// ******************************************************
// * VectorR2 class - math library functions			*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **

const VectorR2 VectorR2::Zero;
// Deprecated due to unsafeness of global initialization
//const VectorR2 VectorR2::UnitX( 1.0, 0.0);
//const VectorR2 VectorR2::UnitY( 0.0, 1.0);
//const VectorR2 VectorR2::NegUnitX(-1.0, 0.0);
//const VectorR2 VectorR2::NegUnitY( 0.0,-1.0);

// Deprecated due to unsafeness of global initialization
//const Matrix2x2 Matrix2x2::Identity(1.0, 0.0, 0.0, 1.0);

// ******************************************************
// * Matrix2x2 class - math library functions			*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **


// ******************************************************
// * LinearMapR2 class - math library functions			*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **


LinearMapR2 LinearMapR2::Inverse() const			// Returns inverse
{


    double detInv = 1.0/(m11*m22 - m12*m21) ;

    return( LinearMapR2( m22*detInv, -m21*detInv, -m12*detInv, m11*detInv ) );
}

LinearMapR2& LinearMapR2::Invert() 			// Converts into inverse.
{
    double detInv = 1.0/(m11*m22 - m12*m21) ;

    double temp;
    temp = m11*detInv;
    m11= m22*detInv;
    m22=temp;
    m12 = -m12*detInv;
    m21 = -m22*detInv;

    return ( *this );
}

VectorR2 LinearMapR2::Solve(const VectorR2& u) const	// Returns solution
{
    // Just uses Inverse() for now.
    return ( Inverse()*u );
}

// ******************************************************
// * RotationMapR2 class - math library functions		*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **



// ***************************************************************
// * 2-space vector and matrix utilities						 *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *




// ***************************************************************
//  Stream Output Routines										 *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ostream& operator<< ( ostream& os, const VectorR2& u )
{
    return (os << "<" << u.x << "," << u.y << ">");
}
