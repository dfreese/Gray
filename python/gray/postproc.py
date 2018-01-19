import numpy as np

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

