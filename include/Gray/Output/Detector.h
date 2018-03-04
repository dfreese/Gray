/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef DETECTOR_H
#define DETECTOR_H
#include "Gray/VrMath/LinearR3.h"
#include "Gray/VrMath/LinearR4.h"

class Detector
{
public:
    Detector();
    Detector(int id, const VectorR3 & p, const VectorR3 &s,
             const RigidMapR3 & m);
    Detector(int id, const VectorR3 & p, const VectorR3 &s,
             const RigidMapR3 & m, int x, int y, int z, int bl);
    void Init(int id, const VectorR3 & p, const VectorR3 &s,
              const RigidMapR3 & m, int x, int y, int z, int bl);
    int detector_id;
    VectorR3 size;
    VectorR3 pos;
    RigidMapR3 map;
    friend ostream& operator<< ( ostream& os, const Detector& d );
    void SetBlock(int bl);
    int idx[3];
    int block;
};

#endif /*DETECTOR_*/
