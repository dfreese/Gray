#!/usr/bin/env python

import os
import sys
import eadl
import xcom
import materials

# See https://stackoverflow.com/a/17510727/2465202 for more information
try:
    # For Python 3.0 and later
    from urllib.request import build_opener
except ImportError:
    # Fall back to Python 2's urllib2
    from urllib2 import build_opener

def usage():
    print('build_materials (materials file) (export directory)')
    print('current directory is used if no export directory is specified')
    print('Gray_Materials.txt in the current directory is used if not specified')

def retreive_dataset(filename, url):
    """
    Download datasets, like the EADL or EPDL on the IAEA website, by blatantly
    spoofing the User-Agent. Spoofing based on this reference:
        http://stackoverflow.com/a/802246/2465202
    Downloading based on this one:
        http://stackoverflow.com/a/22721/2465202
    """
    opener = build_opener()
    opener.addheaders = [('User-Agent', 'Mozilla/5.0')]
    remote_fid = opener.open(url)
    with open(filename, 'w') as local_fid:
        local_fid.write(remote_fid.read())
    remote_fid.close()

def check_dataset(filename=None, url=None):
    """
    Check if a dataset, such as the EADL library, exists, and if it doesn't,
    then use retreive_dataset() to grab it.
    """
    try:
        with open(filename, 'r'):
            exists = True
    except IOError:
        exists = False

    if not exists:
        retreive_dataset(filename, url)


eadl_url = 'https://www-nds.iaea.org/epdl97/data/endfb6/eadl/eadl.all'
epdl_url = 'https://www-nds.iaea.org/epdl97/data/endfb6/epdl97/epdl97.all'
include_xray_escapes = False

# Looks for the include GRAY_INCLUDE variable first, and if it's not set, use
# the current directory.
gray_include_dir = os.getenv('GRAY_INCLUDE', './')
materials_file = gray_include_dir + '/Gray_Materials.txt'
eadl_file = gray_include_dir + '/eadl.all'
epdl_file = gray_include_dir + '/epdl97.all'
export_loc = gray_include_dir

if len(sys.argv) == 3:
    export_loc = sys.argv[2]
    materials_file = sys.argv[1]
elif len(sys.argv) == 2:
    materials_file = sys.argv[1]

gray_mats = materials.load_materials(materials_file)

mat_emis_probs = None
if include_xray_escapes:
    check_dataset(eadl_file, eadl_url)
    check_dataset(epdl_file, epdl_url)
    elements = eadl.EADL(eadl_file)
    elements.read(epdl_file)
    mat_emis_probs = eadl.full_material_emission_probs(gray_mats, elements)

for name, data in gray_mats.items():
    # We grab 11keV to 511keV in steps of 10keV, plus the standard grid
    # which will have all of the important transitions
    energies_mev = [0.001 + x * 0.010 for x in range(52)]
    energy, coh_scat, incoh_scat, photoelec = xcom.compound(data['formula'],
        energies_mev=energies_mev, standard_grid=True, window_min=0.001,
        window_max=1.5)
    if mat_emis_probs is not None:
        emis_probs = mat_emis_probs[name]
    with open(name + '.dat', 'w') as out_fid:
        # Print out the scattering and absorption values first
        out_fid.write('{}\n'.format(len(energy)))
        for e, ics, cs, pe in zip(energy, incoh_scat, coh_scat, photoelec):
            out_fid.write('{0:15.9f}   {1:15.9f}   {2:15.9f}   {3:15.9f}\n'.format(
                e, pe * data['density'], ics * data['density'],
                cs * data['density']))
        # Then add on the flourescent emission
        if mat_emis_probs is not None:
            no_emissions = sum((len(d.keys()) for d in emis_probs.values()))
            print >>out_fid, '%d' % no_emissions
            for bind_e in sorted(emis_probs):
                b_dict = emis_probs[bind_e]
                for e in sorted(b_dict):
                    p = b_dict[e]
                    out_fid.write('{0:15.9f}   {1:15.9f}   {2:15.9f}\n'.format(
                        bind_e, e, p))
