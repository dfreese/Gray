#!/usr/bin/env python

import numpy as np

# Don't align this dtype as #pragma pack(push, 1) was used for the struct
standard_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('pos', np.float32, (3,)),
    ('log', np.int32),
    ('id', np.int32),
    ('det', np.int32)], align=False)

standard_expanded_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('pos', np.float32, (3,)),
    ('interaction', np.int32),
    ('color', np.int32),
    ('scatter', np.int32),
    ('det_mat', np.int32),
    ('src_id', np.int32),
    ('id', np.int32),
    ('det', np.int32)], align=True)

no_position_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('log', np.int32),
    ('id', np.int32),
    ('det', np.int32)], align=True)

no_position_expanded_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('interaction', np.int32),
    ('color', np.int32),
    ('scatter', np.int32),
    ('det_mat', np.int32),
    ('src_id', np.int32),
    ('id', np.int32),
    ('det', np.int32)], align=True)

minimum_dtype = np.dtype([
    ('time', np.float64),
    ('energy', np.float32),
    ('det', np.int32)], align=True)

interaction_fields = (
    ('time', np.float64),
    ('id', np.int32),
    ('color', np.int32),
    ('type', np.int32),
    ('pos', np.float64, (3,)),
    ('energy', np.float64),
    ('det_id', np.int32),
    ('src_id', np.int32),
    ('mat_id', np.int32),
    ('scatter_compton_phantom', np.int32),
    ('scatter_compton_detector', np.int32),
    ('scatter_rayleigh_phantom', np.int32),
    ('scatter_rayleigh_detector', np.int32),
    ('xray_flouresence', np.int32),
    ('coinc_id', np.int32),
    )

interaction_all_dtype = np.dtype(list(interaction_fields))

def create_variable_dtype(write_mask):
    vals = []
    for field, mask in zip(interaction_fields, write_mask):
        if mask:
            vals.append(field)
    return np.dtype(vals)

def variable_field_mask(data):
    '''
    Goes through every field in interaction_fields, and checks if it exists in
    in the data array that was passed.
    '''
    return [f in data.dtype.descr for f in interaction_all_dtype.descr]

def write_variable_binary(filename, data):
    with open(filename, 'wb') as fid:
        fields = np.array(variable_field_mask(data), dtype=np.int32)
        # Magic number, version number, no_fields, no_active, event size
        np.array((65531, 1, fields.size, fields.sum(), data.dtype.itemsize,),
                  dtype=np.int32).tofile(fid)
        fields.tofile(fid)
        data.tofile(fid)

SIGMA_TO_FWHM = 2.0 * np.sqrt(2.0 * np.log(2.0))
FWHM_TO_SIGMA = 1.0 / SIGMA_TO_FWHM

def blur_energy(data, energy_res, ref_energy=None, copy=False):
    '''
    Add a gaussian blur to the energy of the Gray detector output.  Blur is
    computed using:

        e = e * (1 + rand)

    where rand is a Gaussian random variable with a FWHM specified in percent
    by energy_res.

    Parameters
    ----------
    data : ndarray, dtype raw_detector_dtype or detector_dtype
        The array of Gray data
    energy_res : scalar
        Energy resolution (FWHM) of the detectors in percent.
    copy: bool
        If true, a copy of the array is made and returned, otherwise the array
        is modified in place.

    Returns
    -------
    data : ndarray, dtype raw_detector_dtype or detector_dtype
        returns the array, which is a copy or data, modified in place,
        depending on the copy flag.

    '''
    if ref_energy is not None:
        energy_res = energy_res * np.ones(data['energy'].shape)
        valid = (data['energy'] > 0)
        energy_res[valid] = (energy_res * np.sqrt(ref_energy) /
                             np.sqrt(data['energy'][valid]))
    blur = 1.0 + energy_res * FWHM_TO_SIGMA * np.random.randn(data.size)
    if copy:
        data = data.copy()
    data['energy'] *= blur
    return data

def blur_time(data, time_res, copy=False):
    '''
    Add a gaussian blur to the time of the Gray detector output.  Blur is
    computed using:

        t = t + rand

    where rand is a Gaussian random variable with a FWHM specified in seconds.

    Parameters
    ----------
    data : ndarray, dtype raw_detector_dtype or detector_dtype
        The array of Gray data
    time_res : scalar
        Time resolution (FWHM) of the detectors in seconds.
    copy: bool
        If true, a copy of the array is made and returned, otherwise the array
        is modified in place.

    Returns
    -------
    data : ndarray, dtype raw_detector_dtype or detector_dtype
        returns the array, which is a copy or data, modified in place,
        depending on the copy flag.

    '''
    blur = time_res * FWHM_TO_SIGMA * np.random.randn(data.size)
    if copy:
        data = data.copy()
    data['time'] += blur
    return data

def create_log_word(interaction, color, scatter, det_mat, src_id):
    interaction = np.asarray(interaction, dtype=np.int32)
    color = np.asarray(color, dtype=np.int32)
    scatter = np.asarray(scatter, dtype=np.int32)
    det_mat = np.asarray(det_mat, dtype=np.int32)
    src_id = np.asarray(src_id, dtype=np.int32)

    log = np.zeros(interaction.shape, dtype=np.int32)
    np.bitwise_or(np.left_shift(np.bitwise_and(interaction, 0x0000000F), 28),
                  log, out=log)
    np.bitwise_or(np.left_shift(np.bitwise_and(color, 0x0000000F), 24),
                  log, out=log)
    np.bitwise_or(np.left_shift(np.bitwise_and(scatter, 0x000000FF), 20),
                  log, out=log)
    np.bitwise_or(np.left_shift(np.bitwise_and(det_mat, 0x00000FFF), 12),
                  log, out=log)
    np.bitwise_or(np.bitwise_and(src_id, 0x00000FFF),
                  log, out=log)
    return log

def parse_log_word(log):
    log = np.asarray(log, dtype=np.uint32)
    interaction = np.right_shift(
        np.bitwise_and(log, 0xF0000000), 28).astype(np.int32)
    color = np.right_shift(
        np.bitwise_and(log, 0x0F000000), 24).astype(np.int32)
    scatter = np.right_shift(
        np.bitwise_and(log, 0x00F00000), 20).astype(np.int32)
    det_mat = np.right_shift(
        np.bitwise_and(log, 0x000FF000), 12).astype(np.int32)
    src_id = np.bitwise_and(log, 0x00000FFF).astype(np.int32)
    return interaction, color, scatter, det_mat, src_id

def collapse_detector_format(data):
    new_data = np.zeros(data.shape, dtype=no_position_dtype)
    new_data['time'] = data['time']
    new_data['energy'] = data['energy']
    new_data['id'] = data['id']
    new_data['det'] = data['det']
    new_data['log'] = create_log_word(
        data['interaction'], data['color'], data['scatter'], data['det_mat'],
        data['src_id'])
    return new_data

def expand_detector_format(data, full=False):
    if full:
        new_data = np.zeros(data.shape, dtype=standard_expanded_dtype)
    else:
        new_data = np.zeros(data.shape, dtype=no_position_expanded_dtype)
    new_data['time'] = data['time']
    new_data['energy'] = data['energy']
    if full:
        new_data['pos'] = data['pos']
    new_data['id'] = data['id']
    new_data['det'] = data['det']
    (new_data['interaction'], new_data['color'], new_data['scatter'],
     new_data['det_mat'], new_data['src_id']) = parse_log_word(data['log'])
    return new_data

def load_detector_output(filename, full=False, expand=False):
    if full:
        data = np.fromfile(filename, dtype=standard_dtype)
    else:
        data = np.fromfile(filename, dtype=no_position_dtype)

    if expand:
        return expand_detector_format(data, full)
    else:
        return data

def load_variable_binary(filename):
    with open(filename, 'r') as fid:
        magic_number = np.fromfile(fid, dtype=np.int32, count=1)
        if magic_number != 65531:
            RuntimeError('Invalid binary file start')
        version_number = np.fromfile(fid, dtype=np.int32, count=1)
        if version_number != 1:
            RuntimeError('Invalid version number')
        no_fields = np.fromfile(fid, dtype=np.int32, count=1)
        no_active = np.fromfile(fid, dtype=np.int32, count=1)
        event_size = np.fromfile(fid, dtype=np.int32, count=1)
        read_mask = np.fromfile(fid, dtype=np.int32, count=no_fields).astype(bool)
        if read_mask.sum() != no_active:
            RuntimeError('Number of active fields does not match header')
        cur_dtype = create_variable_dtype(read_mask)
        events = np.fromfile(fid, dtype=cur_dtype)
    return events

def load_mapping_file(filename):
    '''
    Load a mapping file used for the gray daq process model into a numpy array
    with a custom dtype to reflect the mapping names.

    Parameters
    ----------
    filename : str,
        The file containing the mapping information

    Returns
    -------
    data : ndarray, dtype custom
        returns the mapping information contained within the file.  The array
        dtype is created based on the column headers and all values are int.

    '''
    with open(filename, 'r') as fid:
        # Read the column names from the first row
        col_names = fid.readline().splitlines()[0].split()
        # Generate a new dtype to represent the mapping file
        map_dtype = np.dtype([(n, int) for n in col_names])
        # and load in the remainder of the rows as the data
        mapping = np.loadtxt(fid, dtype=map_dtype)
    return mapping

def save_mapping_file(filename, mapping):
    '''
    Write a mapping file used for the gray daq process model from a numpy array
    with a custom dtype to reflect the mapping names.

    Parameters
    ----------
    filename : str,
        The filename to which to write the mapping file.
    mapping : ndarray, dtype custom
        The mapping information to be written within the file.  Each name in
        the dtype is used as a column header.  Assumes all types are integers.

    '''
    with open(filename, 'w') as fid:
        print >>fid, ' '.join(mapping.dtype.names)
        np.savetxt(fid, mapping, fmt='%d')
