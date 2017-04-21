import numpy as np

'''
Quick script used to produce detectors/gate_benchmark_fast.map and
detectors/gate_benchmark.map that enable the gate_benchmark_process.pdc
'''

def convert_basic_map(input_filename, output_filename):
    # detector block bx by bz saved from save_basic_map command
    basic_map = np.loadtxt(input_filename, skiprows=1, dtype=int)
    new_det = basic_map[:, 0] // 2 # lso/bgo crystal pair
    new_block = basic_map[:, 1] // 2 # block (with layers paired)
    new_by = basic_map[:, 1] % 2
    new_module = new_block // 25
    new_panel = new_module // 16

    new_map = basic_map[:, np.array((0, 0, 0, 0, 1, 3, 2, 4))]
    new_map[:, 1] = new_det
    new_map[:, 2] = new_block
    new_map[:, 3] = new_module
    new_map[:, 4] = new_panel
    new_map[:, 5] = new_by
    with open(output_filename, 'w') as fid:
        fid.write('crystal detector block module panel bx by bz\n')
        np.savetxt(fid, new_map, fmt='%d')

convert_basic_map('bench_fast.map', 'detectors/gate_benchmark_fast.map')
convert_basic_map('bench.map', 'detectors/gate_benchmark.map')
