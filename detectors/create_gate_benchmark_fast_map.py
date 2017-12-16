#!/usr/bin/env python
import numpy as np
import gray

system_shape = (1,  8, 16, 25, 1,  2)
submodule_shape = (1, 1, 1,)
module_shape = (1, 5, 5,)
rsector_shape = (1, 4, 4,)
no_crystals = np.prod(system_shape)

map_dtype = np.dtype([
    ('detector', int),
    ('crystal', int),
    ('submodule', int),
    ('module', int),
    ('rsector', int),
    ('head', int),
    ('layer', int),
    ('crystal_local', int),
    ('submodule_local', int),
    ('module_local', int),
    ('rsector_local', int),
    ('head_local', int),
    ('bz_crystal', int),
    ('by_crystal', int),
    ('bx_crystal', int),
    ('bz_submodule', int),
    ('by_submodule', int),
    ('bx_submodule', int),
    ('bz_module', int),
    ('by_module', int),
    ('bx_module', int),
    ('axial', int),
    ('transaxial', int),
    ])

# bench_map = np.empty((1,), dtype=map_dtype)
bench_map = np.empty((no_crystals,), dtype=map_dtype)

bench_map['detector'] = np.arange(bench_map.size)
bench_map['crystal'] = bench_map['detector'] // system_shape[5]
bench_map['submodule'] = bench_map['crystal'] // system_shape[4]
bench_map['module'] = bench_map['submodule'] // system_shape[3]
bench_map['rsector'] = bench_map['module'] // system_shape[2]
bench_map['head'] = bench_map['rsector'] // system_shape[1]
bench_map['layer'] = bench_map['detector'] % system_shape[5]
bench_map['crystal_local'] = bench_map['crystal'] % system_shape[4]
bench_map['submodule_local'] = bench_map['submodule'] % system_shape[3]
bench_map['module_local'] = bench_map['module'] % system_shape[2]
bench_map['rsector_local'] = bench_map['rsector'] % system_shape[1]
bench_map['head_local'] = bench_map['head'] % system_shape[0]
bench_map['bz_crystal'] = bench_map['crystal_local'] // submodule_shape[1] 
bench_map['by_crystal'] = bench_map['crystal_local'] % submodule_shape[1] 
bench_map['bx_crystal'][:] = 0
bench_map['bz_submodule'] = bench_map['submodule_local'] // module_shape[1] 
bench_map['by_submodule'] = bench_map['submodule_local'] % module_shape[1] 
bench_map['bx_submodule'][:] = 0
bench_map['bz_module'] = bench_map['module_local'] // rsector_shape[1] 
bench_map['by_module'] = bench_map['module_local'] % rsector_shape[1] 
bench_map['bx_module'][:] = 0
bench_map['axial'] = bench_map['bz_crystal'] + submodule_shape[2] * (
    bench_map['bz_submodule'] + module_shape[2] * bench_map['bz_module'])
bench_map['transaxial'] = bench_map['by_crystal'] + submodule_shape[1] * (
    bench_map['by_submodule'] + module_shape[1] * (bench_map['by_module'] +
        bench_map['rsector'] * rsector_shape[1])
        )


gray.save_mapping_file('gate_benchmark_fast.map', bench_map)
