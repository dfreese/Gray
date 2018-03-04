/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Physics/BackBack.h"
#include <limits>

using namespace std;

BackBack::BackBack(double acolinearity_deg_fwhm) :
    Positron(acolinearity_deg_fwhm,
                  std::numeric_limits<double>::infinity()) // Infinite half-life
{
    Reset();
}
