/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef THOMPSON_H
#define THOMPSON_H

#include <vector>

class Thompson {
public:
    static double dsigma(const double costheta);
    static std::vector<double> dsigma(const std::vector<double>& costheta);
};

#endif // THOMPSON_H
