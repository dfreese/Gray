import urllib
import re

_NIST_XCOM_FORM = 'https://physics.nist.gov/cgi-bin/Xcom/data.pl'

def compound(formula, energies_mev=None, standard_grid=False, window_min=0.001,
             window_max=100000, return_energies=False, return_scatter=True,
             return_photo=True, return_pair_prod=False, return_atten=False):
    """
    Gets material cross-section data from the NIST XCOM website, based on a
    given chemical formula, given as a string.  The energies in MeV can be
    given as an iterable of numbers, or as a string with spaces, commas, or
    newlines as a delimeter.

    This returns lists of floats for each of the types are specified.  The
    values that are returned and the flags that enable or disable them are:

        * Energies (MeV), return_energies
        * Coherent Scattering (cm^2/g), return_scatter
        * Incoherent Scattering (cm^2/g), return_scatter
        * Photoelectron Absorption (cm^2/g), return_photo
        * Pair Production in Nuclear Field (cm^2/g), return_pair_prod
        * Pair Production in Electron Field (cm^2/g)), return_pair_prod
        * Attenuation with Coherent Scattering (cm^2/g), return_atten
        * Attenuation without Coherent Scattering (cm^2/g), return_atten

    This work was initially based on the nistxcom work shown here:
    https://github.com/davepeake/XRay_Attenuation/
    """
    if not isinstance(formula, str):
        raise TypeError('Formula should be a string')

    if isinstance(energies_mev, str):
        # If the energies are given in the form of a string, handle any number
        # of delimiters, and store them back to energies as a list as per:
        # http://stackoverflow.com/a/4998688/2465202
        energies_mev = re.split('; |, |\*|\n', energies_mev)

    # The xcom data request requires the energies to be listed as text,
    # separated by a newline.
    if energies_mev is not None:
        energies_mev = '\n'.join(str(x) for x in energies_mev)
    else:
        energies_mev = ''

    postdict = {'Name':'',
                'Method':'2',
                'Formula':formula,
                'NumAdd':'1',
                'Energies':energies_mev,
                'Output':'',
                'WindowXmin':str(window_min),
                'WindowXmax':str(window_max),
                'character':'space',
                'coherent':'on',
                'incoherent':'on',
                'photoelectric':'on',
                'nuclear':'on',
                'electron':'on',
                'with':'on',
                'without':'on'}

    if standard_grid:
        postdict['Output'] = 'on'
        return_energies = True

    postdata = urllib.urlencode(postdict)
    xcom_url = urllib.urlopen(_NIST_XCOM_FORM, postdata)
    xcom_data = xcom_url.read().splitlines()

    energies = []
    coherent_scatter = []
    incoherent_scatter = []
    photoelectric = []
    nuclear_pair_prod = []
    electron_pair_prod = []
    atten_with_coherent = []
    atten_without_coherent = []
    for line in xcom_data[3:]:
        data = line.split()
        energies.append(float(data[0]))
        coherent_scatter.append(float(data[1]))
        incoherent_scatter.append(float(data[2]))
        photoelectric.append(float(data[3]))
        nuclear_pair_prod.append(float(data[4]))
        electron_pair_prod.append(float(data[5]))
        atten_with_coherent.append(float(data[6]))
        atten_without_coherent.append(float(data[7]))

    output = []
    if return_energies:
        output.append(energies)
    if return_scatter:
        output.append(coherent_scatter)
        output.append(incoherent_scatter)
    if return_photo:
        output.append(photoelectric)
    if return_pair_prod:
        output.append(nuclear_pair_prod)
        output.append(electron_pair_prod)
    if return_atten:
        output.append(atten_with_coherent)
        output.append(atten_without_coherent)

    if len(output) == 1:
        output = output[0]
    return output
