import gray
import os
import subprocess
import tempfile
import numpy as np

def _run_merge(filename, output_filename, map_filename, proc_filename,
               input_dtype, verbose):
    cmd = 'gray-daq -f %s -o %s -m %s -p %s' % (
        filename, output_filename, map_filename, proc_filename
    )
    if verbose:
        cmd += ' -v'
    if input_dtype == gray.standard_dtype:
        cmd += ' -t 0'
    elif input_dtype == gray.no_position_dtype:
        cmd += ' -t 1'
    subprocess.call([cmd], shell=True)

def _create_test_process_file(map_osfid, cmd_lines):
    if not isinstance(cmd_lines, list):
        cmd_lines = [cmd_lines,]
    with os.fdopen(map_osfid, 'wb') as map_fid:
        for cmd_line in cmd_lines:
            if len(cmd_line) == 3:
                cmd, comp, time = cmd_line
                options = None
            else:
                cmd, comp, time, options = cmd_line
            if options is None:
                line = '%s %s %g' % (cmd, comp, time)
            else:
                line = '%s %s %g %s' % (cmd, comp, time, options)
            map_fid.write(line)
            map_fid.write('\n')

def _create_test_map_file(map_osfid, no_det):
    id_map = np.zeros((no_det, 5), dtype=int)
    id_map[:, 0] = np.arange(no_det)
    id_map[:, 1] = id_map[:, 0] // 64
    id_map[:, 2] = (id_map[:, 0] % 64) // 8
    id_map[:, 3] = (id_map[:, 0] % 64) % 8
    id_map[:, 4] = 0
    with os.fdopen(map_osfid, 'wb') as map_fid:
        map_fid.write('detector block bx by bz\n')
        np.savetxt(map_fid, id_map, '%d')

def _create_and_run_merge(data, cmd_lines, verbose=False):
    input_osfid, input_fname = tempfile.mkstemp()
    with os.fdopen(input_osfid, 'wb') as input_fid:
        data.tofile(input_fid)

    map_osfid, map_fname = tempfile.mkstemp()
    _create_test_map_file(map_osfid, data['det'].max() + 1)

    proc_osfid, proc_fname = tempfile.mkstemp()
    _create_test_process_file(proc_osfid, cmd_lines)

    output_osfid, output_fname = tempfile.mkstemp()
    _run_merge(input_fname, output_fname, map_fname, proc_fname, data.dtype,
               verbose)
    out = np.fromfile(output_fname, dtype=data.dtype)
    os.remove(input_fname)
    os.remove(output_fname)
    os.remove(map_fname)
    os.remove(proc_fname)
    return out

def test_merge_outside_window():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window
    data[1]['energy'] = 250.0

    output = _create_and_run_merge(data, ('merge', 'detector', merge_window))
    assert(output.size == data.size), \
            'Size should remain unchanged for event outside of merge window'
    assert((output == data).all()), '''return data should be unchanged as it
           is outside the merge window'''

def test_merge_inside_window():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 250.0

    output = _create_and_run_merge(data, ('merge', 'detector', merge_window))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == (data[0]['energy'] + data[1]['energy'])), \
           ''' Energy should be the sum of the input'''

    comments = _create_and_run_merge(data, ('merge', 'detector', merge_window,
                                            ' # first'))
    assert(output.size == comments.size), \
        'comment in the options should have been ignored'
    assert((output == comments).all()), \
         'comment in the options should have been ignored'

def test_merge_inside_window_basic_flag():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 250.0

    output = _create_and_run_merge(data, ('merge', 'detector', merge_window))
    flagged = _create_and_run_merge(data, ('merge', 'detector', merge_window,
                                    'max'))

    assert(output.size == flagged.size), 'Max flag should be default'
    assert((output == flagged).all()), 'Max flag should be default'

def test_merge_different_detectors():
    data = np.zeros(3, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[2]['time'] = merge_window - 1e-6
    data[2]['energy'] = 250.0
    data[1]['time'] = merge_window / 3.0 * 2.0
    data[1]['det'] = 1

    output = _create_and_run_merge(data, ('merge', 'detector', merge_window))
    assert(output.size == 2), 'Events should have been merged to two events'
    assert(output[0]['energy'] == (data[0]['energy'] + data[2]['energy'])), \
           ''' Energy should be the sum of the input'''
    assert(output[0]['time'] == data[0]['time']), 'Time should be unchanged'
    assert(output[1] == data[1]), 'Second event should be unchanged'

def test_deadtime():
    data = np.zeros(7, dtype=gray.no_position_dtype)
    deadtime = 100.0

    times = np.array((0.0, 50.0, 75.0, 174.99, 300.0, 399.999, 400.0),
                        dtype=data.dtype['time'])
    data['time'] = times
    expected = data[np.array((0, 3, 4, 6))]
    output = _create_and_run_merge(data, ('deadtime', 'detector', deadtime))
    check = _create_and_run_merge(data, ('deadtime', 'detector', deadtime,
                                         'nonparalyzable'))

    assert(output.size == expected.size), \
            'Expected number of events not cut for nonparalyzable deadtime'
    assert((output == expected).all()), \
           'Event times are not as expected for nonparalyzable deadtime'

    assert(output.size == check.size), \
            'nonparalyzable deadtime should be the default'
    assert((output == check).all()), \
           'nonparalyzable deadtime should be the default'

    expected = data[np.array((0, 4))]
    output = _create_and_run_merge(data, ('deadtime', 'detector', deadtime,
                                          'paralyzable'))

    assert(output.size == expected.size), \
        'Expected number of events not cut for paralyzable deadtime'
    assert((output == expected).all()), \
        'Event times are not as expected for paralyzable deadtime'

def test_egate_low():
    data = np.zeros(5, dtype=gray.no_position_dtype)
    egate_low = 100.0

    energies = np.array((99.9999, 100.0, 200.0, 300.0, 300.001),
                        dtype=data.dtype['energy'])
    data['energy'] = energies

    output = _create_and_run_merge(data, ('filter', 'egate_low', egate_low))
    assert(output.size == (energies >= egate_low).sum()), \
            'One event should have been cut'
    assert((output['energy'] == energies[energies >= egate_low]).all()), \
           '''Energies above the threshold are not correct'''

def test_egate_high():
    data = np.zeros(5, dtype=gray.no_position_dtype)
    egate_high = 300.0

    energies = np.array((99.9999, 100.0, 200.0, 300.0, 300.001),
                        dtype=data.dtype['energy'])
    data['energy'] = energies

    output = _create_and_run_merge(data, ('filter', 'egate_high', egate_high))
    assert(output.size == (energies <= egate_high).sum()), \
            'One event should have been cut'
    assert((output['energy'] == energies[energies <= egate_high]).all()), \
           '''Energies above the threshold are not correct'''

def test_eblur():
    data = np.zeros(5, dtype=gray.no_position_dtype)
    eres = 0.13

    energies = np.array((99.9999, 100.0, 200.0, 300.0, 300.001),
                        dtype=data.dtype['energy'])
    data['energy'] = energies

    output = _create_and_run_merge(data, ('blur', 'energy', eres))
    assert(output.size == data.size), \
            'Size should remain unchanged for eres blur'
    assert((output['energy'] != energies).any()), \
            'At least one energy should change with blurring (statistically)'

def test_sort():
    data = np.zeros(5, dtype=gray.no_position_dtype)
    max_time = 30.0
    times = np.array((10.0, 0.0, 20.0, 50.0, 60.0),
                        dtype=data.dtype['time'])
    data['time'] = times

    output = _create_and_run_merge(data, ('sort', 'time', max_time))
    assert(output.size == data.size), \
            'Size should remain unchanged for sort'
    assert((output['time'] == times[np.argsort(times)]).all()), \
            'Time should be sorted'

def test_tblur():
    data = np.zeros(5, dtype=gray.no_position_dtype)
    tres = 2.0

    times = np.array((0.0, 10.0, 20.0, 50.0, 60.0),
                        dtype=data.dtype['time'])
    data['time'] = times

    output = _create_and_run_merge(data, ('blur', 'time', tres))
    assert(output.size == data.size), \
            'Size should remain unchanged for time blur'
    assert((output['time'] != times).any()), \
            'At least one energy should change with blurring (statistically)'
    assert((np.abs(output['time'] - times) < 3 * tres).all()), \
            'The blur should be capped to 3 FWHM in either direction'

def test_coinc():
    data = np.zeros(7, dtype=gray.no_position_dtype)
    coinc_win = 15.0
    times = np.array((0.0, 10.0, 24.999, 50.0, 65.0, 90.0, 100.0),
                     dtype=data.dtype['time'])
    data['time'] = times

    expected = data[np.array((0, 1, 5, 6))]
    output = _create_and_run_merge(data, ('coinc', 'window', coinc_win))

    assert(output.size == expected.size), \
            'Size not expected for nonparalyzable coinc sort'
    assert((output == expected).all()), \
            'Event data not as expected for nonparalyzable coinc sort'
    alt_out = _create_and_run_merge(data, ('coinc', 'nonparalyzable',
                                           coinc_win))
    assert(output.size == alt_out.size), \
            'nonparalyzable and window for coinc should have same effect'
    assert((output == alt_out).all()), \
            'nonparalyzable and window for coinc should have same effect'

    expected = data[np.array((5, 6))]
    output = _create_and_run_merge(data, ('coinc', 'paralyzable', coinc_win))

    assert(output.size == expected.size), \
            'Size not expected for paralyzable coinc sort'
    assert((output == expected).all()), \
            'Event data not as expected for paralyzable coinc sort'

    expected = data[np.array((0, 1, 2, 5, 6))]
    output = _create_and_run_merge(data, ('coinc', 'paralyzable', coinc_win,
                                          'keep_multiples'))
    assert(output.size == expected.size), \
            'Size not expected for paralyzable coinc sort keeping mutiples'
    assert((output == expected).all()), \
            '''Event data not as expected for paralyzable coinc sort keeping
            mutiples'''

def test_delayed_window():
    data = np.zeros(7, dtype=gray.no_position_dtype)
    coinc_win = 15.0
    times = np.array((0.0, 10.0, 24.999, 50.0, 65.0, 90.0, 100.0),
                     dtype=data.dtype['time'])
    data['time'] = times

    expected = _create_and_run_merge(data, ('coinc', 'window', coinc_win))
    output = _create_and_run_merge(data, ('coinc', 'window', coinc_win,
                                          'delay 0.0'))
    assert(output.size == expected.size), \
            'Delayed window offset of zero should not change the output'
    assert((output == expected).all()), \
            'Delayed window offset of zero should not change the output'

    delay_window = 50.0
    expected = data[np.array((0, 1, 3, 4, 5, 6))]
    output = _create_and_run_merge(data, ('coinc', 'window', coinc_win,
                                          'delay ' + str(delay_window)))

    assert(output.size == expected.size), \
            'Size not expected for nonparalyzable delayed window'
    assert((output == expected).all()), \
            'Event data not as expected for nonparalyzable delayed window'

    delay_window = 50.0
    expected = data[np.array((0, 1, 3, 4, 5, 6))]
    output = _create_and_run_merge(data, ('coinc', 'paralyzable', coinc_win,
                                          'delay ' + str(delay_window)))

    assert(output.size == expected.size), \
            'Size not expected for paralyzable delayed window'
    assert((output == expected).all()), \
            'Event data not as expected for paralyzable delayed window'


    delay_window = 50.0
    expected = data[np.array((0, 1, 2, 3, 4, 5, 6))]
    output = _create_and_run_merge(data, ('coinc', 'paralyzable', coinc_win,
                                          'keep_multiples delay '
                                          + str(delay_window)))

    assert(output.size == expected.size), \
            '''Size not expected for paralyzable delayed window keeping
            multiples'''
    assert((output == expected).all()), \
            '''Event data not as expected for paralyzable delayed window
            keeping multiples'''



    data = np.zeros(5, dtype=gray.no_position_dtype)
    times = np.array((0.0, 55.0, 145.0, 195.0, 200.0),
                     dtype=data.dtype['time'])
    data['time'] = times

    delay_window = 50.0
    expected = data[np.array((0, 1, 3, 4))]
    output = _create_and_run_merge(data, ('coinc', 'window', coinc_win,
                                          'delay ' + str(delay_window)))

    assert(output.size == expected.size), \
            'Size not expected for nonparalyzable delayed window'
    assert((output == expected).all()), \
            'Event data not as expected for nonparalyzable delayed window'

    expected = data[np.array((0, 1, 2, 3, 4))]
    output = _create_and_run_merge(data, ('coinc', 'window', coinc_win,
                                          'keep_multiples delay ' +
                                          str(delay_window)))

    assert(output.size == expected.size), \
            '''Size not expected for nonparalyzable delayed window keeping
            multiples'''
    assert((output == expected).all()), \
            '''Event data not as expected for nonparalyzable delayed window
            keeping multiples'''

def test_multiple_merge_egate_coinc():
    data = np.zeros(8, dtype=gray.no_position_dtype)
    merge_window = 10.0
    coinc_win = 15.0
    egate_low = 400.0
    times = np.array((0.0, 10.0, 19.999, 50.0, 54.0, 60.0, 69.999, 100.0),
                     dtype=data.dtype['time'])
    detectors = np.array((0, 1, 1, 2, 2, 64, 65, 80),
                         dtype=data.dtype['det'])
    energies = np.array((511.0, 200.0, 311.0, 200.0, 199.999, 511.0, 400.0,
                         511.0),
                        dtype=data.dtype['energy'])
    data['time'] = times
    data['det'] = detectors
    data['energy'] = energies

    expected = data[np.array((0, 1, 5, 6))]
    expected['energy'][1] += energies[2]

    output = _create_and_run_merge(data, [('merge', 'detector', merge_window),
                                          ('filter', 'egate_low', egate_low),
                                          ('coinc', 'window', coinc_win)])

    assert(output.size == expected.size), \
            'Size not expected for coinc sort'
    assert((output == expected).all()), \
            'Event data not as expected for coinc sort'


def test_merge_array_outside_window():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window
    data[1]['energy'] = 250.0

    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'array_eweight bx by bz 8 8 1'))
    assert((output == data).all()), '''return data should be unchanged as it
           is outside the merge window'''

def test_merge_array_inside_window():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 250.0

    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'max'))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == (data[0]['energy'] + data[1]['energy'])), \
           ''' Energy should be the sum of the input'''

def test_merge_array_different_arrays():
    data = np.zeros(3, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[2]['time'] = merge_window - 1e-6
    data[2]['energy'] = 250.0
    data[1]['time'] = merge_window / 3.0 * 2.0
    data[1]['det'] = 64

    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'array_eweight bx by bz 8 8 1'))
    assert(output.size == 2), 'Events should have been merged to two events'
    assert(output[0]['energy'] == (data[0]['energy'] + data[2]['energy'])), \
           'Energy should be the sum of the input'
    assert(output[0]['time'] == data[0]['time']), 'Time should be unchanged'
    assert(output[1] == data[1]), 'Second event should be unchanged'

def test_merge_array_basic():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 250.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 300.0
    data[1]['det'] = 24 + 5

    new_energy = data[0]['energy'] + data[1]['energy']
    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'first'))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == data[0]['det']), \
           'New detector should the first in time'

def test_merge_array_basic_type0():
    data = np.zeros(2, dtype=gray.standard_dtype)
    merge_window = 300.0

    data[0]['energy'] = 250.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 300.0
    data[1]['det'] = 24 + 5

    new_energy = data[0]['energy'] + data[1]['energy']
    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'first'))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == data[0]['det']), \
           'New detector should the first in time'

def test_merge_array_basic_type0_default():
    data = np.zeros(2, dtype=gray.standard_dtype)
    merge_window = 300.0

    data[0]['energy'] = 250.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 300.0
    data[1]['det'] = 24 + 5

    new_energy = data[0]['energy'] + data[1]['energy']
    output = _create_and_run_merge(data, ('merge', 'block', merge_window))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == data[1]['det']), \
           'New detector should be from the largest energy'

def test_merge_array_max():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 250.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 300.0
    data[1]['det'] = 24 + 5

    new_energy = data[0]['energy'] + data[1]['energy']
    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'max'))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == data[1]['det']), \
           'New detector should the second with the max energy'

def test_merge_array_weighted_mean_same_col():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 250.0
    data[1]['det'] = 7

    new_energy = data[0]['energy'] + data[1]['energy']
    # We've chosen crystals in the same row, so they should be a straight
    # linear combination
    new_det = int(data[0]['det'] * (data[0]['energy'] / new_energy) +
                  data[1]['det'] * (data[1]['energy'] / new_energy))

    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'array_eweight bx by bz 8 8 1'))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == new_det), \
           'New detector should be a weighted linear combination of the two'

def test_merge_array_weighted_mean_same_row():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 250.0
    data[1]['det'] = 24

    new_energy = data[0]['energy'] + data[1]['energy']
    # We've chosen crystals in the same row, so they should be a straight
    # linear combination
    new_det = 8 * int(data[0]['det'] // 8 * (data[0]['energy'] / new_energy) +
                      data[1]['det'] // 8 * (data[1]['energy'] / new_energy))

    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'array_eweight bx by bz 8 8 1'))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == new_det), \
           'New detector should be a weighted linear combination of the two'

def test_merge_array_weighted_mean():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 250.0
    data[1]['det'] = 24 + 5

    new_energy = data[0]['energy'] + data[1]['energy']
    # We've chosen crystals in the same row, so they should be a straight
    # linear combination
    new_col = int(data[0]['det'] // 8 * (data[0]['energy'] / new_energy) +
                  data[1]['det'] // 8 * (data[1]['energy'] / new_energy))
    new_row = int(data[0]['det'] % 8 * (data[0]['energy'] / new_energy) +
                  data[1]['det'] % 8 * (data[1]['energy'] / new_energy))
    new_det = 8 * new_col + new_row

    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'array_eweight bx by bz 8 8 1'))
    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == new_det), \
           'New detector should be a weighted mean of the two'
