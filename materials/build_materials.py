#!/usr/bin/env python

import datetime
import json
import os
import sys
import epdl

def usage():
    print('build_materials (materials file) (export directory)\n'
          'current directory is used if no export directory is specified\n'
          'GrayMaterials.db in the current directory is used if not specified\n')

def md5sum(filename):
    import hashlib
    hasher = hashlib.md5()
    with open(filename, 'rb') as fid:
        buf = fid.read()
        hasher.update(buf)
    return hasher.hexdigest()

# Looks for the include GRAY_INCLUDE variable first, and if it's not set, use
# the current directory.
gray_include_dir = os.getenv('GRAY_INCLUDE', '.')
materials_file = gray_include_dir + '/GrayMaterials.db'
export_loc = gray_include_dir
export_file = export_loc + '/GrayPhysics.json'

if len(sys.argv) == 3:
    export_loc = sys.argv[2]
    materials_file = sys.argv[1]
elif len(sys.argv) == 2:
    materials_file = sys.argv[1]

materials = epdl.gate_database_materials(materials_file, 0.001, 1.5)

output = {}
output['materials'] = {n: m.to_dict() for n, m in materials.items()}
output['info'] = {
    'date_created': datetime.datetime.now().isoformat(),
    'materials_md5': md5sum(materials_file),
    }

with open(export_file, 'w') as fid:
    json.dump(output, fid, indent=4, sort_keys=True)
