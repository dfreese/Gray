import gray
import os
import subprocess
import tempfile
import numpy as np

_sigma_to_fwhm = 2.0 * np.sqrt(2.0 * np.log(2.0))
_fwhm_to_sigma = 1.0 / _sigma_to_fwhm

def _run_merge(filename, output_filename, map_filename, proc_filename,
               input_dtype, verbose, coinc_filenames=None):
    cmd = 'gray-daq -h %s -s %s -m %s -p %s' % (
        filename, output_filename, map_filename, proc_filename
    )
    if verbose:
        cmd += ' -v'
    if input_dtype == gray.standard_dtype:
        cmd += ' -i full_binary'
    elif input_dtype == gray.no_position_dtype:
        cmd += ' -i no_pos_binary'
    if coinc_filenames is not None:
        for coinc_name in coinc_filenames:
            cmd += ' -c %s' % coinc_name
    assert(subprocess.call([cmd], shell=True) == 0)

def _create_test_process_file(map_osfid, cmd_lines):
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

def _create_and_run_merge(data, cmd_lines, verbose=False, clear_files=True):
    if not isinstance(cmd_lines, list):
        cmd_lines = [cmd_lines,]
    input_osfid, input_fname = tempfile.mkstemp()
    with os.fdopen(input_osfid, 'wb') as input_fid:
        data.tofile(input_fid)

    map_osfid, map_fname = tempfile.mkstemp()
    _create_test_map_file(map_osfid, (data['det'].max() // 64 + 1) * 64)

    proc_osfid, proc_fname = tempfile.mkstemp()
    _create_test_process_file(proc_osfid, cmd_lines)

    output_osfid, output_fname = tempfile.mkstemp()

    coinc_osfids = []
    coinc_fnames = []
    for cmd_line in cmd_lines:
        if cmd_line[0] == 'coinc':
            coinc_osfid, coinc_fname = tempfile.mkstemp()
            coinc_osfids.append(coinc_osfid)
            coinc_fnames.append(coinc_fname)

    _run_merge(input_fname, output_fname, map_fname, proc_fname, data.dtype,
               verbose, coinc_fnames)

    out = np.fromfile(output_fname, dtype=data.dtype)
    coinc_outs = []
    for name in coinc_fnames:
        coinc_outs.append(np.fromfile(name, dtype=data.dtype))
        os.remove(name)

    if clear_files:
        os.remove(input_fname)
        os.remove(output_fname)
        os.remove(map_fname)
        os.remove(proc_fname)
    if len(coinc_outs) == 0:
        return out
    else:
        return [out,] + coinc_outs

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

def test_merge_across_read_boundary():
    read_interval = 100000 # gray-daq reads this many events at a time.
    data = np.zeros(2 * read_interval, dtype=gray.no_position_dtype)
    merge_window = 10.0

    data['time'] = np.arange(0, data.size * (merge_window / 2), (merge_window / 2))

    output = _create_and_run_merge(data, ('merge', 'detector', merge_window))
    assert(output.size == data.size / 2), \
        'Half of events should have been merged'

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

def test_merge_different_detectors_interspersed():
    data = np.zeros(5, dtype=gray.no_position_dtype)
    merge_window = 300.0

    times = np.array((0, merge_window / 3.0 * 2.0, merge_window - 1e-6,
                      merge_window / 3.0 * 5.0 - 1e-6,
                      merge_window / 3.0 * 2.0 + merge_window))
    dets = np.array((0, 1, 0, 1, 1))
    energies = np.array((300.0, 500.0, 250.0, 11.0, 40.0))
    data['time'] = times
    data['det'] = dets
    data['energy'] = energies

    expected = data[np.array((0, 1, 4))]
    expected['energy'][np.array((0, 1))] += data['energy'][np.array((2, 3))]

    output = _create_and_run_merge(data, ('merge', 'detector', merge_window))

    assert(output.size == expected.size), \
            'Size not expected for interspersed merge'
    assert((output == expected).all()), \
            'Event data not as expected for interspersed merge'


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

def test_energy_blur():
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

def test_energy_blur_std():
    data = np.zeros(100000, dtype=gray.no_position_dtype)
    ref_energy = 0.511
    eres = 0.13
    data['energy'][:] = ref_energy
    output = _create_and_run_merge(data, ('blur', 'energy', eres, 'at ' + str(ref_energy)))
    std_out = output['energy'].std()
    eres_out = std_out * _sigma_to_fwhm / ref_energy
    assert(np.abs(eres - eres_out) / eres < 1e-2)

    energy = ref_energy / 2
    data['energy'][:] = energy
    output = _create_and_run_merge(data, ('blur', 'energy', eres, 'at ' + str(ref_energy)))
    std_out = output['energy'].std()
    eres_out = std_out * _sigma_to_fwhm / energy
    exp_eres = eres * np.sqrt(ref_energy) / np.sqrt(energy)
    assert(np.abs(exp_eres - eres_out) / exp_eres < 1e-2)

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

def test_time_blur():
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

def test_time_blur_std():
    data = np.zeros(100000, dtype=gray.no_position_dtype)

    tres_sigma = 2.0

    # Make sure the data is sorted and stays sorted by putting it more than
    # 3 sigma apart.  Sort expects a partially, or mostly sorted input, so we
    # can't give it a bunch of zeros without taking a lot of time.
    data['time'] = np.arange(0, data.size * 4 * tres_sigma, 4 * tres_sigma)

    tres = tres_sigma * _sigma_to_fwhm
    output = _create_and_run_merge(data, ('blur', 'time', tres))
    std_out = (output['time'] - data['time']).std()
    assert(np.abs(tres_sigma - std_out) / tres_sigma < 1e-2)

def test_coinc():
    data = np.zeros(7, dtype=gray.no_position_dtype)
    coinc_win = 15.0
    times = np.array((0.0, 10.0, 24.999, 50.0, 65.0, 90.0, 100.0),
                     dtype=data.dtype['time'])
    data['time'] = times

    expected = data[np.array((0, 1, 5, 6))]
    [singles, output] = _create_and_run_merge(data, ('coinc', 'window', coinc_win))

    assert(output.size == expected.size), \
            'Size not expected for nonparalyzable coinc sort'
    assert((output == expected).all()), \
            'Event data not as expected for nonparalyzable coinc sort'

    expected = data[np.array((5, 6))]
    [singles, output] = _create_and_run_merge(data, ('coinc', 'window', coinc_win, 'paralyzable'))

    assert(output.size == expected.size), \
            'Size not expected for paralyzable coinc sort'
    assert((output == expected).all()), \
            'Event data not as expected for paralyzable coinc sort'

    # pair all of the first three together.
    expected = data[np.array((0, 1, 0, 2, 1, 2, 5, 6))]
    [singles, output] = _create_and_run_merge(data, ('coinc', 'window', coinc_win,
                                          'keep_multiples paralyzable'))
    assert(output.size == expected.size), \
            'Size not expected for paralyzable coinc sort keeping mutiples'
    assert((output == expected).all()), \
            '''Event data not as expected for paralyzable coinc sort keeping
            mutiples'''

def test_delayed_window():
    data = np.zeros(9, dtype=gray.no_position_dtype)
    coinc_win = 15.0
    times = np.array((0.0, 10.0, 24.999, 50.0, 65.0, 90.0, 100.0, 160.0, 161.0),
                     dtype=data.dtype['time'])
    data['time'] = times

    [singles, expected] = _create_and_run_merge(data, [
        ('coinc', 'window', coinc_win),])
    [singles, output] = _create_and_run_merge(data, ('coinc', 'delay', coinc_win, '0.0'))
    assert(output.size == expected.size), \
            'Delayed window offset of zero should not change the output'
    assert((output == expected).all()), \
            'Delayed window offset of zero should not change the output'

    delay_window = 50.0
    expected = data[np.array((0, 3, 1, 4))]
    [singles, output] = _create_and_run_merge(data, [
        ('coinc', 'delay', coinc_win, str(delay_window)),
        ])

    assert(output.size == expected.size), \
            'Size not expected for nonparalyzable delayed window'
    assert((output == expected).all()), \
            'Event data not as expected for nonparalyzable delayed window'

    delay_window = 50.0
    expected = data[np.array((0, 3, 1, 4))]
    [singles, output] = _create_and_run_merge(data, [
        ('coinc', 'delay', coinc_win, str(delay_window) + ' paralyzable'),])

    assert(output.size == expected.size), \
            'Size not expected for paralyzable delayed window'
    assert((output == expected).all()), \
            'Event data not as expected for paralyzable delayed window'


    delay_window = 50.0
    expected = data[np.array((0, 3, 1, 4, 6, 7, 6, 8, 7, 8))]
    [singles, output] = _create_and_run_merge(data, [
        ('coinc', 'delay', coinc_win, str(delay_window) + ' keep_multiples'),
        ])

    assert(output.size == expected.size), \
            '''Size not expected for delayed window keeping multiples'''
    assert((output == expected).all()), \
            '''Event data not as expected for paralyzable delayed window
            keeping multiples'''


    data = np.zeros(5, dtype=gray.no_position_dtype)
    times = np.array((0.0, 55.0, 145.0, 195.0, 200.0),
                     dtype=data.dtype['time'])
    data['time'] = times

    delay_window = 50.0
    expected_coinc = data[np.array((3, 4))]
    expected_delay = data[np.array((0, 1))]
    [singles, coinc, delays] = _create_and_run_merge(data, [
        ('coinc', 'window', coinc_win),
        ('coinc', 'delay', coinc_win, str(delay_window)),
        ])

    assert(delays.size == expected_delay.size), \
            'Size not expected for nonparalyzable delayed window'
    assert((delays == expected_delay).all()), \
            'Event data not as expected for nonparalyzable delayed window'

    assert(coinc.size == expected_coinc.size), \
            'Size not expected for nonparalyzable coinc window'
    assert((coinc == expected_coinc).all()), \
            'Event data not as expected for nonparalyzable coinc window'


    expected = data[np.array((0, 1, 2, 3, 2, 4, 3, 4))]
    [singles, output] = _create_and_run_merge(data, [
        ('coinc', 'delay', coinc_win, str(delay_window) + ' keep_multiples'),])

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

    [singles, output] = _create_and_run_merge(data, [
        ('merge', 'detector', merge_window, 'first'),
        ('filter', 'egate_low', egate_low),
        ('coinc', 'window', coinc_win)])

    assert(output.size == expected.size), \
            'Size not expected for coinc sort'
    assert((output == expected).all()), \
            'Event data not as expected for coinc sort'

    # Now data[1] and data[2] will be merged into data[2], but this will put
    # the event outside of the coincidence time window.
    expected = data[np.array((5, 6))]
    [singles, output] = _create_and_run_merge(data, [
        ('merge', 'detector', merge_window,),
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
                                   'anger bx by bz'))
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
                                   'anger bx by bz'))
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
                                   'anger bx by bz'))
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
                                   'anger bx by bz'))
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

    array_size = 64
    column_size = 8
    new_energy = data[0]['energy'] + data[1]['energy']
    det0_local = data[0]['det'] % array_size
    det1_local = data[1]['det'] % array_size
    # We've chosen crystals in the same row, so they should be a straight
    # linear combination
    new_col = int(det0_local // column_size * (data[0]['energy'] / new_energy) +
                  det1_local // column_size * (data[1]['energy'] / new_energy))
    new_row = int(det0_local % column_size * (data[0]['energy'] / new_energy) +
                  det1_local % column_size * (data[1]['energy'] / new_energy))
    new_det = ((data['det'][0] // array_size) * array_size +
               column_size * new_col + new_row)

    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'anger bx by bz'))

    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == new_det), \
           'New detector should be a weighted mean of the two'


def test_merge_array_weighted_mean_next_array():
    data = np.zeros(2, dtype=gray.no_position_dtype)
    merge_window = 300.0

    data[0]['energy'] = 300.0
    data[1]['time'] = merge_window - 1e-6
    data[1]['energy'] = 250.0
    data[1]['det'] = 24 + 5

    array_size = 64
    column_size = 8
    data['det'] += array_size
    new_energy = data[0]['energy'] + data[1]['energy']
    det0_local = data[0]['det'] % array_size
    det1_local = data[1]['det'] % array_size
    # We've chosen crystals in the same row, so they should be a straight
    # linear combination
    new_col = int(det0_local // column_size * (data[0]['energy'] / new_energy) +
                  det1_local // column_size * (data[1]['energy'] / new_energy))
    new_row = int(det0_local % column_size * (data[0]['energy'] / new_energy) +
                  det1_local % column_size * (data[1]['energy'] / new_energy))
    new_det = ((data['det'][0] // array_size) * array_size +
               column_size * new_col + new_row)

    output = _create_and_run_merge(data, ('merge', 'block', merge_window,
                                   'anger bx by bz'))

    assert(output.size == 1), 'Events should have been merged to one'
    assert(output[0]['energy'] == new_energy), \
           'Energy should be the sum of the input'
    assert(output[0]['det'] == new_det), \
           'New detector should be a weighted mean of the two'


def test_stats_merge():
    data = np.zeros(3, dtype=gray.no_position_expanded_dtype)
    merge_window = 100.0

    time = np.array((0.0, 50.0, 75.0,), dtype=data.dtype['time'])
    scatter = np.array((1, 3, 2), dtype=data.dtype['scatter'])
    color = np.array((0, 1, 0), dtype=data.dtype['color'])
    data['time'] = time
    data['scatter'] = scatter
    data['color'] = color

    data_small = gray.collapse_detector_format(data)
    assert((data == gray.expand_detector_format(data_small)).all())

    expected = data[np.array((0,))]
    expected[0]['scatter'] = 5
    output = _create_and_run_merge(data_small, [
        ('merge', 'detector', merge_window),])
    output = gray.expand_detector_format(output)

    assert(output.size == expected.size), \
            'Expected number of events not merged stats merge test'
    assert((output == expected).all()), \
           'Number of scatters not calculated correctly'
