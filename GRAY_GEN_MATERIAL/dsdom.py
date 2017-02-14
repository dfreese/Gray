#!/usr/bin/env python

import sys

energies = (
    10.000,
    15.000,
    20.000,
    30.000,
    40.000,
    50.000,
    60.000,
    80.000,
    100.000,
    150.000,
    200.000,
    300.000,
    400.000,
    500.000,
    511.000,
    600.000)

LUT = (
    6.731,
    6.679,
    6.628,
    6.532,
    6.442,
    6.356,
    6.273,
    6.121,
    5.982,
    5.677,
    5.420,
    5.006,
    4.682,
    4.417,
    4.390,
    4.194)

def dsdom(energy):
    energy = float(energy)
    if energy < energies[0]:
        return LUT[0]
    elif energy >= energies[-1]:
        return LUT[-1]
    else:
        for ii in range(len(energies)):
            if energy < energies[ii]:
                delta = energies[ii] - energies[ii - 1];
                alpha = (energy - energies[ii - 1]) / delta;
                return (1.0 - alpha) * LUT[ii - 1] + alpha * LUT[ii];

if __name__ == '__main__':
    print '%.6f' % dsdom(float(sys.argv[1]))
