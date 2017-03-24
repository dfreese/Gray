#!/usr/bin/env python

import os
import sys
import xcom
import materials

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

gray_mats = materials.load_materials(materials_file)

for name, data in gray_mats.iteritems():
    # We grab 11keV to 511keV in steps of 10keV, plus the standard grid
    # which will have all of the important transitions
    energies_mev = [0.011 + x * 0.010 for x in range(51)]
    energy, coh_scat, incoh_scat, photoelec = xcom.compound(data['formula'],
        energies_mev=energies_mev, standard_grid=True, window_min=0.011,
        window_max=1.5)
    with open(name + '.dat', 'w') as out_fid:
        print >>out_fid, '%d' % len(energy)
        last_e = 0.0
        for e, ics, cs, pe in zip(energy, incoh_scat, coh_scat, photoelec):
            if e == last_e:
                # If we've come across a transition, we add a small offset
                # to the following edge to make sure Gray's linear
                # interpolation doesn't choke.
                e += 0.0000025
            last_e = e
            print >>out_fid, '%15.9f   %15.9f   %15.9f   %15.9f' % (
                e, pe * data['density'], ics * data['density'],
                cs * data['density'])
