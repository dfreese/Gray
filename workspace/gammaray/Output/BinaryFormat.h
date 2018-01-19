/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef BINARYFORMAT_H
#define BINARYFORMAT_H

#include <stdint.h>

#pragma pack(push, 1)

struct GrayBinaryStandard {
    double time;
    float energy;
    float x,y,z;
    int log;
    int i;
    int det_id;
};
#pragma pack(pop)

struct GrayBinaryNoPosition {
    double time;
    float energy;
    int log;
    int i;
    int det_id;
};

#endif /*BINARYFORMAT_*/
