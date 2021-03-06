#!/usr/bin/env python

import datetime
import json
import os
import sys
import epdl
import gray

def usage():
    print('build_materials.py:\n'
          '\n'
          'Current directory is used if GRAY_INCLUDE is not set\n'
          'Builds GrayPhysics.json from GrayMaterials.db, GrayIsotopes.txt,\n'
          'GraySensitive.txt\n')

def md5sum(filename):
    import hashlib
    hasher = hashlib.md5()
    with open(filename, 'rb') as fid:
        buf = fid.read()
        hasher.update(buf)
    return hasher.hexdigest()

# TODO: take these arguments from the command line
# TODO: throw appropriate more appropriate errors if files are not found
# Looks for the include GRAY_INCLUDE variable first, and if it's not set, use
# the current directory.
gray_include_dir = os.getenv('GRAY_INCLUDE', os.getcwd())
materials_file = os.path.join(gray_include_dir, 'GrayMaterials.db')
isotopes_file = os.path.join(gray_include_dir, 'GrayIsotopes.txt')
sensitive_file = os.path.join(gray_include_dir, 'GraySensitive.txt')
export_file = os.path.join(gray_include_dir, 'GrayPhysics.json')

materials = gray.database_epdl(materials_file, 0.001, 1.5)
sensitive_mats = gray.read_sensitive_file(sensitive_file)

output = {}
output['materials'] = {n: m.to_dict() for n, m in materials.items()}
gray.mark_sensitve(output['materials'], sensitive_mats)
output['isotopes'] = gray.read_isotopes_file(isotopes_file)
output['info'] = {
    'date_created': datetime.datetime.now().isoformat(),
    'materials_md5': md5sum(materials_file),
    'isotopes_md5': md5sum(isotopes_file),
    'sensitive_md5': md5sum(sensitive_file),
    'density_unit': 'g/cm3',
    'energy_unit': 'MeV',
    'linear_atten_unit': '1/cm',
    }

with open(export_file, 'w') as fid:
    json.dump(output, fid, indent=4, sort_keys=True)
