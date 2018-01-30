/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

//  Consider a function which satisfies the following properties:
//    1.   f(N) = 1 + A*f(alpha*N) + B*f(beta*N)
//           where A+B > 1 and alpha, beta in (0,1].
//    2    f(1) = 1.
//
//  These conditions are satisfied by setting f(x) = C N^exponent + D,
//    where C, e, D are the values returned by FindDoubleRecurseSoln

void FindDoubleRecurseSoln ( double A, double B, double alpha, double beta,
                             double* C, double* exponent, double* D );

// In the second form of FindDoubleRecurseSoln, there is an "exponentToBeat"
//		parameter.  If the value of "exponent" would be less than "exponentToBeat",
//		then the function returns FALSE, and the values of C, exponent and D
//		are not returned as valid value.

bool FindDoubleRecurseSoln ( double A, double B, double alpha, double beta,
                             double* C, double* exponent, double* D,
                             double exponentToBeat );
