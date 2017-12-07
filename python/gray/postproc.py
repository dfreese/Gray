import numpy as np
from gray.dtypes import (
    no_position_dtype,
    no_position_expanded_dtype,
    )

def sigma_to_fwhm():
    return 2.0 * np.sqrt(2.0 * np.log(2.0))

def fwhm_to_sigma():
    return 1.0 / sigma_to_fwhm()

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
