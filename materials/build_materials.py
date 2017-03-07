#!/usr/bin/env python

import os
import sys
import xcom

def usage():
    print 'build_materials (materials file) (export directory)'
    print 'current directory is used if no export directory is specified'
    print 'Gray_Materials.txt in the current directory is used if not specified'

# Looks for the include GRAY_INCLUDE variable first, and if it's not set, use
# the current directory.
gray_include_dir = os.getenv('GRAY_INCLUDE', './')
materials_file = gray_include_dir + '/Gray_Materials.txt'
export_loc = gray_include_dir

if len(sys.argv) == 3:
    export_loc = sys.argv[2]
    materials_file = sys.argv[1]
elif len(sys.argv) == 2:
    materials_file = sys.argv[1]

with open(materials_file, 'r') as fid:
    for line in fid.read().splitlines():
        # Ignore blank lines and lines that start with a #
        if len(line) == 0:
            continue
        elif line[0] == '#':
            continue
        # Assume five columns, separated by spaces  The first three are
        # relevant to this script
        name, formula, density, sensitive = line.split()
        density = float(density)
        # We grab 11keV to 511keV in steps of 10keV, plus the standard grid
        # which will have all of the important transitions
        energies_mev = [0.011 + x * 0.010 for x in range(51)]
        energy, incoh_scat, coh_scat, photoelec = xcom.compound(formula,
            energies_mev=energies_mev, standard_grid=True, window_min=0.011,
            window_max=1.5)
        with open(name + '.dat', 'w') as out_fid:
            print >>out_fid, '%d' % len(energy)
            last_e = 0.0
            for e, cs, pe in zip(energy, coh_scat, photoelec):
                if e == last_e:
                    # If we've come across a transition, we add a small offset
                    # to the following edge to make sure Gray's linear
                    # interpolation doesn't choke.
                    e += 0.0000025
                last_e = e
                print >>out_fid, '%15.9f   %15.9f   %15.9f' % (
                    e * 1000, pe * density, cs * density)
